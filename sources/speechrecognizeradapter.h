#ifndef SPEECHRECOGNIZERADAPTER_H
#define SPEECHRECOGNIZERADAPTER_H

#include "speechrecognizer.h"

namespace Moqt
{


class SpeechRecognizerAdapter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SpeechRecognizerAdapter)

public:
    explicit SpeechRecognizerAdapter(SpeechRecognizer* parent);

    virtual void requestAuthorization() = 0;
    virtual void startListening(const QStringList& expectedPhrases) = 0;
    virtual void stopListening() = 0;

    const SpeechRecognizer* speechRecognizer() const { return m_speechRecognizer; }

    // May be called on another thread
    void setAuthorized(bool v);
    // May be called on another thread
    void partialResultsReady(QStringList texts); // An array of potential transcriptions, sorted in descending order of confidence.
    // May be called on another thread
    void resultsReady(QStringList texts);  // An array of potential transcriptions, sorted in descending order of confidence.
    // May be called on another thread
    void error();
    // May be called on another thread
    void setAvailability(bool available, bool availableOnDevice);

private:
    SpeechRecognizer* m_speechRecognizer;
};


}


#endif // SPEECHRECOGNIZERADAPTER_H
