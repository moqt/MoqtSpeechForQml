#include "speechrecognizeradapter.h"

#include <QTimer>
#include <QDebug>


namespace Moqt
{


class MockSpeechRecognizerAdapter : public SpeechRecognizerAdapter
{
public:
    using SpeechRecognizerAdapter::SpeechRecognizerAdapter;

    void requestAuthorization() override
    {
         setAuthorized(true);
         setAvailability(true, true);
    }

    void startListening(const QStringList& expectedPhrases) override
    {
        startTimer(m_partialResultsReadyTimer, 2000, expectedPhrases, true);
        startTimer(m_resultsReadyTimer, 4000, expectedPhrases, false);
    }

    void stopListening() override
    {
        m_partialResultsReadyTimer.disconnect();
        m_partialResultsReadyTimer.stop();
        m_resultsReadyTimer.disconnect();
        m_resultsReadyTimer.stop();
    }

private:
    QTimer m_resultsReadyTimer;
    QTimer m_partialResultsReadyTimer;

    void startTimer(QTimer& timer, int interval, QStringList expectedPhrases, bool partialResults) {
        timer.setInterval(interval);
        timer.start();
        timer.callOnTimeout([this, expectedPhrases, partialResults, &timer] () {
            timer.disconnect(); // disconnect everything
            if (expectedPhrases.isEmpty()) {
                error();
            } else {
                if (partialResults) {
                    qDebug() << "MockSpeechRecognizerAdapter - partialResultsReady:" << expectedPhrases;
                    partialResultsReady(expectedPhrases);
                } else {
                    qDebug() << "MockSpeechRecognizerAdapter - resultsReady:" << expectedPhrases;
                    resultsReady(expectedPhrases);
                }
            }
        });
    }
};


void SpeechRecognizer::initAdapter()
{
    m_adapter = new MockSpeechRecognizerAdapter(this);
}


}
