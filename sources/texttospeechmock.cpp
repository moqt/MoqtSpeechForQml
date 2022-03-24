#include "texttospeechadapter.h"

#include <QTimer>


namespace Moqt
{


class MockTextToSpeechAdapter : public TextToSpeechAdapter
{
public:
    explicit MockTextToSpeechAdapter(TextToSpeech* parent) : TextToSpeechAdapter(parent)
    {
        setInitialized(true);
    }

    void speak(QString text, QJSValue utteranceIdOrFinishCallback) override
    {
        playSilence(static_cast<int>(text.length()) * 30 + 200, utteranceIdOrFinishCallback);
    }

    void playSilence(int duraion, QJSValue utteranceIdOrFinishCallback) override
    {
        m_timer.setInterval(duraion);
        m_timer.start();
        m_timer.callOnTimeout([utteranceIdOrFinishCallback, this] () {
            m_timer.disconnect(); // disconnect everything
            speakFinished(utteranceIdOrFinishCallback);
        });
    }

    void pause() override
    {
        m_timer.stop();
    }

    void resume() override
    {
        m_timer.start();
    }

private:
    QTimer m_timer;
};



void TextToSpeech::initAdapter()
{
    m_adapter = new MockTextToSpeechAdapter(this);
}


}
