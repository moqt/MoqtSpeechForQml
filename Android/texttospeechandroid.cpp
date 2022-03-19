#include "texttospeechadapter.h"

#include <QJniObject>
#include <QCoreApplication>
#include <QMap>


namespace
{

void jSpeakFinished(JNIEnv *env, jobject thiz, jlong cppPointer, jstring jUtteranceId);


void jSetInitialized(JNIEnv *env, jobject thiz, jlong cppPointer, jboolean v)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    auto cppAdapter = reinterpret_cast<Moqt::TextToSpeechAdapter*>(cppPointer);
    cppAdapter->setInitialized(v);
}


void registerNatveMethods()
{
    static bool registered;
    if (!registered) {
        const JNINativeMethod methods[] = {
                        {"speakFinished", "(JLjava/lang/String;)V", reinterpret_cast<void *>(jSpeakFinished)},
                        {"setInitialized", "(JZ)V", reinterpret_cast<void *>(jSetInitialized)}
                     };
        QJniEnvironment env;
        env.registerNativeMethods("org/moqt/speech/TextToSpeechAdapter", methods, sizeof(methods) / sizeof(methods[0]));
        registered = true;
    }
}

}


namespace Moqt
{

class TextToSpeechAndroidAdapter : public TextToSpeechAdapter
{
public:
    TextToSpeechAndroidAdapter(TextToSpeech* parent) : TextToSpeechAdapter(parent),
        m_javaAdapter("org/moqt/speech/TextToSpeechAdapter",
                      "(Landroid/content/Context;JLjava/lang/String;FFFLjava/lang/String;)V",
                      QNativeInterface::QAndroidApplication::context(),
                      reinterpret_cast<jlong>(this),
                      QJniObject::fromString(parent->language()).object<jstring>(),
                      static_cast<jfloat>(parent->pitch()),
                      static_cast<jfloat>(parent->rate()),
                      static_cast<jfloat>(parent->volume()),
                      QJniObject::fromString(parent->voice()).object<jstring>())
    {
        registerNatveMethods();
    }

    ~TextToSpeechAndroidAdapter() override
    {
        m_javaAdapter.callMethod<void>("shutdown");
    }

    void speak(QString text, QJSValue utteranceIdOrFinishCallback) override
    {
        const QString utteranceId = insertToUtteranceIdOrFinishCallbackStorage(utteranceIdOrFinishCallback);
        m_javaAdapter.callMethod<void>("speak",
                                       "(Ljava/lang/String;Ljava/lang/String;)V",
                                       QJniObject::fromString(text).object<jstring>(),
                                       QJniObject::fromString(utteranceId).object<jstring>());
    }

    void playSilence(int duraion, QJSValue utteranceIdOrFinishCallback) override
    {
        const QString utteranceId = insertToUtteranceIdOrFinishCallbackStorage(utteranceIdOrFinishCallback);
        m_javaAdapter.callMethod<void>("playSilence",
                                       "(JLjava/lang/String;)V",
                                       duraion,
                                       QJniObject::fromString(utteranceId).object<jstring>());
    }

    void pause() override
    {
        m_javaAdapter.callMethod<void>("pause");
    }

    void resume() override
    {
        m_javaAdapter.callMethod<void>("resume");
    }

    void speakFinishedWithUtteranceId(const QString& utteranceId)
    {
        const QJSValue utteranceIdOrFinishCallback = m_utteranceIdOrFinishCallbackStorage.take(utteranceId);
        speakFinished(utteranceIdOrFinishCallback);
    }

private:
    QJniObject m_javaAdapter;
    QMap<QString, QJSValue> m_utteranceIdOrFinishCallbackStorage;

    QString insertToUtteranceIdOrFinishCallbackStorage(QJSValue utteranceIdOrFinishCallback)
    {
        const QString utteranceId = utteranceIdOrFinishCallback.toString(); // TODO test is it correct for callback?
        m_utteranceIdOrFinishCallbackStorage.insert(utteranceId, utteranceIdOrFinishCallback);
        return utteranceId;
    }
};


void TextToSpeech::initAdapter()
{
    qDebug() << "Create TextToSpeechAndroidAdapter";
    m_adapter = new TextToSpeechAndroidAdapter(this);
}


}



namespace
{

void jSpeakFinished(JNIEnv *env, jobject thiz, jlong cppPointer, jstring jUtteranceId)
{
    Q_UNUSED(thiz)

    auto utteranceId = env->GetStringUTFChars(jUtteranceId, nullptr);
    auto cppAdapter = reinterpret_cast<Moqt::TextToSpeechAndroidAdapter*>(cppPointer);
    cppAdapter->speakFinishedWithUtteranceId(QString::fromUtf8(utteranceId));
    env->ReleaseStringUTFChars(jUtteranceId, utteranceId);
}

}
