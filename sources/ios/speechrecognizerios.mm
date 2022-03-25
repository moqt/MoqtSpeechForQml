#include "speechrecognizeradapter.h"

#import <Speech/Speech.h>

#include <QDebug>



namespace
{


bool availableOnDevice(SFSpeechRecognizer *speechRecognizer)
{
    bool availableOnDevice = false;
    if (@available(iOS 13.0, *)) {
        availableOnDevice = speechRecognizer.supportsOnDeviceRecognition;
    }
    return availableOnDevice;
}


NSMutableArray* NSMutableArrayFromQStringList(const QStringList& qstringList)
{
    NSMutableArray* arr = [[NSMutableArray alloc] init];
    for (const auto& qs : qstringList) {
        [arr addObject:qs.toNSString()];
    }
    return arr;
}


QStringList QStringListFromTranscriptions(NSArray<SFTranscription *>* transcriptions)
{
    QStringList qstringList;
    for (SFTranscription* t in transcriptions) {
        qstringList.append(QString::fromNSString(t.formattedString));
    }
    return qstringList;
}


}



@interface MoqtSpeechRecognizerDelegate : NSObject<SFSpeechRecognizerDelegate>
@end

@implementation MoqtSpeechRecognizerDelegate
{
    Moqt::SpeechRecognizerAdapter* m_cppOwner;
}

-(id) initWithOwner:(Moqt::SpeechRecognizerAdapter *)cppOwner
{
    if ((self = [super init]) != nil) {
        m_cppOwner = cppOwner;
    }
    return self;
}

// We want to override dealloc, just to see whether the objects we create will get deallocated correctly.
- (void)dealloc
{
    NSLog(@"MoqtSpeechRecognizerDelegate was destroyed");
    [super dealloc];
}

#pragma mark - SFSpeechRecognizerDelegate Protocol
- (void)speechRecognizer:(SFSpeechRecognizer *)speechRecognizer availabilityDidChange:(BOOL)available {
    NSLog(@"availabilityDidChange:%d", available);

    m_cppOwner->setAvailability(available, availableOnDevice(speechRecognizer));
}

@end


namespace Moqt
{


class SpeechRecognizerIosAdapter : public SpeechRecognizerAdapter
{
public:
    explicit SpeechRecognizerIosAdapter(SpeechRecognizer* parent) : SpeechRecognizerAdapter(parent)
    {
    }

    ~SpeechRecognizerIosAdapter() override
    {
        //stopListening();
        releaseListeningObjects();
        [m_recognizer release];
        [m_delegate release];
    }

    void requestAuthorization() override
    {
        // TODO maxResults not taken into account
        m_delegate = [[MoqtSpeechRecognizerDelegate alloc] initWithOwner:this];
        NSString* language = speechRecognizer()->language().toNSString();

        [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus authStatus) {
            switch (authStatus) {
                case SFSpeechRecognizerAuthorizationStatusAuthorized:
                    //User gave access to speech recognition
                    NSLog(@"SFSpeechRecognizerAuthorized");
                    setAuthorized(true);

                    m_recognizer = [[SFSpeechRecognizer alloc] initWithLocale:[NSLocale localeWithLocaleIdentifier:language]];
                    if (m_recognizer) {
                        m_recognizer.delegate = m_delegate;
                        // availabilityDidChange is not called after SFSpeechRecognizer init, so set it directly
                        setAvailability(m_recognizer.available, availableOnDevice(m_recognizer));
                    } else {
                        NSLog(@"Could not create speech recognizer for language: %@", language);
                        error();
                    }
                    break;

                case SFSpeechRecognizerAuthorizationStatusDenied:
                    //User denied access to speech recognition
                    NSLog(@"SFSpeechRecognizerAuthorizationStatusDenied");
                    error();
                    break;

                case SFSpeechRecognizerAuthorizationStatusRestricted:
                    //Speech recognition restricted on this device
                    NSLog(@"SFSpeechRecognizerAuthorizationStatusRestricted");
                    break;

                case SFSpeechRecognizerAuthorizationStatusNotDetermined:
                    //Speech recognition not yet authorized
                    break;
            }
        }];
    }

    void startListening(const QStringList& expectedPhrases) override
    {
        if (m_task) {
            qWarning() << "Listening already started in speech recognizer";
            return;
        }

        if (!m_recognizer) {
            qWarning() << "Speech recognizer is not authorized, call requestAuthorization()";
            error();
            return;
        }

        m_audioEngine = [[AVAudioEngine alloc] init];

        // Starts an AVAudio Session
        NSError *error = nil;
        AVAudioSession *audioSession = [AVAudioSession sharedInstance];
        [audioSession setCategory:AVAudioSessionCategoryRecord error:&error];
        NSLog(@"audioSession setCategory, error: %@.", error);
        [audioSession setActive:YES withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:&error];
        NSLog(@"audioSession setActive, error: %@.", error);

        m_recognitionRequest = [[SFSpeechAudioBufferRecognitionRequest alloc] init];
        m_recognitionRequest.shouldReportPartialResults = YES;
        if (@available(iOS 13.0, *)) {
            m_recognitionRequest.requiresOnDeviceRecognition = speechRecognizer()->preferOnDeviceRecognition();
        }
        m_recognitionRequest.contextualStrings = NSMutableArrayFromQStringList(expectedPhrases);

        m_task = [m_recognizer recognitionTaskWithRequest:m_recognitionRequest resultHandler:^(SFSpeechRecognitionResult * _Nullable result, NSError * _Nullable error) {
            if (result) {
                NSLog(@"Speech recognizer best result: %@", result.bestTranscription.formattedString);
                m_lastResults = QStringListFromTranscriptions(result.transcriptions);
                if (result.isFinal) {
                    qDebug() << "Speech recognizer results" << m_lastResults;
                    resultsReady(m_lastResults);
                    releaseListeningObjects();
                } else {
                    qDebug() << "Speech recognizer partial results" << m_lastResults;
                    partialResultsReady(m_lastResults);
                }
            }
            if (error) {
                NSLog(@"Speech recognizer error: %@", error);
                if (m_task) {
                    if (error.code == 203) { // No input after partial results
                        qDebug() << "Speech recognizer results (use the last)" << m_lastResults;
                        resultsReady(m_lastResults);
                    } else if (error.code == 1110) { // No input at all
                        qDebug() << "Speech recognizer empty results";
                        resultsReady(QStringList());
                    } else {
                        this->error();
                    }
                    releaseListeningObjects();
                }
            }
        }];

        AVAudioInputNode *inputNode = m_audioEngine.inputNode;
        AVAudioFormat *recordingFormat = [inputNode outputFormatForBus:0];
        [inputNode installTapOnBus:0 bufferSize:1024 format:recordingFormat block:^(AVAudioPCMBuffer * _Nonnull buffer, AVAudioTime * _Nonnull when) {
            [m_recognitionRequest appendAudioPCMBuffer:buffer];
        }];

        [m_audioEngine prepare];
        [m_audioEngine startAndReturnError:&error];
        NSLog(@"Audio engine started, error: %@. Say Something, speech recognizer is listening...", error);
    }

    void stopListening() override
    {
        [m_audioEngine stop];
        [m_recognitionRequest endAudio];

//        [m_task finish];
//        AVAudioInputNode *inputNode = m_audioEngine.inputNode;
//        [inputNode removeTapOnBus: 0];
//        [inputNode reset];
    }

private:
    SFSpeechRecognizer* m_recognizer = nil;
    MoqtSpeechRecognizerDelegate* m_delegate = nil;
    AVAudioEngine* m_audioEngine = nil;
    SFSpeechAudioBufferRecognitionRequest* m_recognitionRequest = nil;
    SFSpeechRecognitionTask* m_task = nil;
    QStringList m_lastResults;

    void releaseListeningObjects()
    {
        [m_audioEngine release];
        m_audioEngine = nil;

        //[m_task release];
        m_task = nil;

        [m_recognitionRequest release];
        m_recognitionRequest = nil;

        m_lastResults.clear();
    }
};


void SpeechRecognizer::initAdapter()
{
    m_adapter = new SpeechRecognizerIosAdapter(this);
}


}
