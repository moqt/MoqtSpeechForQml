#ifndef TEXTTOSPEECH_H
#define TEXTTOSPEECH_H

#include <QObject>
#include <QVariant>
#include <QJSValue>
#include <QQmlParserStatus>
#include <QtQml/qqmlregistration.h>

namespace Moqt
{

class TextToSpeechAdapter;


class TextToSpeech : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(bool speaking READ speaking NOTIFY speakingChanged FINAL)
    Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged FINAL)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged FINAL)
    Q_PROPERTY(QString language MEMBER m_language NOTIFY languageChanged FINAL) // For example "en-GB"
    Q_PROPERTY(qreal pitch MEMBER m_pitch NOTIFY pitchChanged FINAL)
    Q_PROPERTY(qreal rate MEMBER m_rate NOTIFY rateChanged FINAL)
    Q_PROPERTY(qreal volume MEMBER m_volume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(QString voiceId MEMBER m_voiceId NOTIFY voiceIdChanged FINAL)
    // TODO list of available voices

    Q_DISABLE_COPY_MOVE(TextToSpeech)

public:
    explicit TextToSpeech(QObject *parent = nullptr);

    bool speaking() const;
    bool initialized() const;

    bool paused() const;
    void setPaused(bool v);

    const QString& language() const { return m_language; }
    qreal pitch() const { return m_pitch; }
    qreal rate() const { return m_rate; }
    qreal volume() const { return m_volume; }
    const QString& voice() const { return m_voiceId; }

    void classBegin() override;
    void componentComplete() override;

public slots:
    void speak(const QString& text, const QJSValue& utteranceIdOrFinishCallback);
    void playSilence(int duraion, const QJSValue& utteranceIdOrFinishCallback);

signals:
    void speakingChanged();
    void initializedChanged();
    void pausedChanged();
    void speakFinished(QJSValue utteranceIdOrCallback);

    void languageChanged();
    void pitchChanged();
    void rateChanged();
    void volumeChanged();
    void voiceIdChanged();

private slots:
    void setInitialized(bool v);

private:
    TextToSpeechAdapter* m_adapter = nullptr;

    QString m_language;
    qreal m_pitch = 1;
    qreal m_rate = 1;
    qreal m_volume = 1;
    QString m_voiceId;
    bool m_paused = false;
    bool m_speaking = false;
    bool m_initialized = false;

    void initAdapter();
    void setSpeaking(bool v);
};

}

#endif // TEXTTOSPEECH_H
