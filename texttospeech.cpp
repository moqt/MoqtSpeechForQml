#include "texttospeechadapter.h"


namespace Moqt
{


TextToSpeechAdapter::TextToSpeechAdapter(TextToSpeech* parent)
    : QObject(parent), m_textToSpeech(parent)
{
}


void TextToSpeechAdapter::speakFinished(QJSValue utteranceIdOrCallback)
{
    QMetaObject::invokeMethod(m_textToSpeech, "speakFinished", Qt::QueuedConnection,
                              Q_ARG(QJSValue, utteranceIdOrCallback));
}


void TextToSpeechAdapter::setInitialized(bool v)
{
    QMetaObject::invokeMethod(m_textToSpeech, "setInitialized", Qt::QueuedConnection,
                              Q_ARG(bool, v));
}



TextToSpeech::TextToSpeech(QObject *parent)
    : QObject{parent}
{
    QObject::connect(this, &TextToSpeech::speakFinished, [this] (QJSValue utteranceIdOrCallback) {
        setSpeaking(false);
        if (utteranceIdOrCallback.isCallable()) {
            utteranceIdOrCallback.call();
        }
    });
}


void TextToSpeech::classBegin()
{
}


void TextToSpeech::componentComplete()
{
    Q_ASSERT(!m_adapter);
    initAdapter();
}


bool TextToSpeech::speaking() const
{
    return m_speaking;
}


void TextToSpeech::setSpeaking(bool v)
{
    if (v != m_speaking) {
        m_speaking = v;
        emit speakingChanged();
    }
}


bool TextToSpeech::initialized() const
{
    return m_initialized;
}


void TextToSpeech::setInitialized(bool v)
{
    if (v != m_initialized) {
        m_initialized = v;
        emit initializedChanged();
    }
}


bool TextToSpeech::paused() const
{
    return m_paused;
}


void TextToSpeech::setPaused(bool v)
{
    if (v != m_paused) {
        m_paused = v;
        emit pausedChanged();

        if (m_adapter) {
            if (m_paused) {
                m_adapter->pause();
            } else {
                m_adapter->resume();
            }
        }
    }
}


void TextToSpeech::speak(const QString& text, const QJSValue& utteranceIdOrFinishCallback)
{
    if (m_adapter) {
        setSpeaking(true);
        m_adapter->speak(text, utteranceIdOrFinishCallback);
    }
}


void TextToSpeech::playSilence(int duraion, const QJSValue& utteranceIdOrFinishCallback)
{
    if (m_adapter) {
        setSpeaking(true);
        m_adapter->playSilence(duraion, utteranceIdOrFinishCallback);
    }
}


}
