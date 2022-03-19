#include "texttospeechadapter.h"

#import <AVFoundation/AVFoundation.h>


@interface MoqtSpeechSynthesizerDelegate : NSObject<AVSpeechSynthesizerDelegate>
    @property(nonatomic) QJSValue utteranceIdOrFinishCallback;
@end

@implementation MoqtSpeechSynthesizerDelegate
{
    Moqt::TextToSpeechAdapter* m_cppOwner;
}

-(id) initWithOwner:(Moqt::TextToSpeechAdapter *)cppOwner
{
    if ((self = [super init]) != nil) {
        m_cppOwner = cppOwner;
    }
    return self;
}

// We want to override dealloc, just to see whether the objects we create will get deallocated correctly.
// Note that we are using ARC and thus a call to [super dealloc] is forbidden (and unnecessary).
- (void)dealloc
{
    NSLog(@"MoqtSpeechSynthesizerDelegate was destroyed");
}

#pragma mark - AVSpeechSynthesizerDelegate Protocol
- (void)speechSynthesizer:(AVSpeechSynthesizer *)synthesizer didFinishSpeechUtterance:(AVSpeechUtterance *)utterance
{
    m_cppOwner->speakFinished(self.utteranceIdOrFinishCallback);
}

@end


namespace Moqt
{


class TextToSpeechIosAdapter : public TextToSpeechAdapter
{
public:
    explicit TextToSpeechIosAdapter(TextToSpeech* parent) : TextToSpeechAdapter(parent)
    {
        m_synthesizer = [[AVSpeechSynthesizer alloc] init];
        m_synthesizer.delegate = m_delegate = [[MoqtSpeechSynthesizerDelegate alloc] initWithOwner:this];
        setInitialized(true);
    }

    ~TextToSpeechIosAdapter() override
    {
        [m_synthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
        [m_synthesizer release];
        [m_delegate release];
    }

    void speak(QString text, QJSValue utteranceIdOrFinishCallback) override
    {
        AVSpeechUtterance *utterance = [AVSpeechUtterance speechUtteranceWithString:text.toNSString()];
        utterance.rate = static_cast<float>(textToSpeech()->rate() * AVSpeechUtteranceDefaultSpeechRate);
        utterance.pitchMultiplier = static_cast<float>(textToSpeech()->pitch());
        utterance.volume = static_cast<float>(textToSpeech()->volume());
        if (!textToSpeech()->language().isEmpty()) {
            utterance.voice = [AVSpeechSynthesisVoice voiceWithLanguage:textToSpeech()->language().toNSString()];
        }
        m_delegate.utteranceIdOrFinishCallback = utteranceIdOrFinishCallback;
        [m_synthesizer speakUtterance:utterance];
    }

    void playSilence(int duraion, QJSValue utteranceIdOrFinishCallback) override
    {
        AVSpeechUtterance *utterance = [AVSpeechUtterance speechUtteranceWithString:@""];
        utterance.preUtteranceDelay = 0.001 * duraion;
        m_delegate.utteranceIdOrFinishCallback = utteranceIdOrFinishCallback;
        [m_synthesizer speakUtterance:utterance];
    }

    void pause() override
    {
        [m_synthesizer pauseSpeakingAtBoundary:AVSpeechBoundaryImmediate];
    }

    void resume() override
    {
        [m_synthesizer continueSpeaking];
    }

private:
    AVSpeechSynthesizer* m_synthesizer;
    MoqtSpeechSynthesizerDelegate* m_delegate;
};



void TextToSpeech::initAdapter()
{
    m_adapter = new TextToSpeechIosAdapter(this);
}


}
