#ifndef TEXTTOSPEECHADAPTER_H
#define TEXTTOSPEECHADAPTER_H

#include "texttospeech.h"

namespace Moqt
{


class TextToSpeechAdapter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TextToSpeechAdapter)

public:
    explicit TextToSpeechAdapter(TextToSpeech* parent);

    virtual void speak(QString text, QJSValue utteranceIdOrFinishCallback) = 0;
    virtual void playSilence(int duraion, QJSValue utteranceIdOrFinishCallback) = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;

    const TextToSpeech* textToSpeech() const { return m_textToSpeech; }

    // May be called on another thread
    void setInitialized(bool v);
    // May be called on another thread
    void speakFinished(QJSValue utteranceIdOrCallback);

private:
    TextToSpeech* m_textToSpeech;
};


}


#endif // TEXTTOSPEECHADAPTER_H
