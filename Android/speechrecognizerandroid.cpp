#include "speechrecognizeradapter.h"

#include <QCoreApplication>
#include <QMap>
#include <QJniEnvironment>
#include <QJniObject>


namespace
{

QStringList jobjectToQStringList(jobject jo)
{
    QStringList result;
    QJniEnvironment env;
    QJniObject o(jo);

    const jsize size = env->GetArrayLength(o.object<jarray>());
    for (int i = 0; i < size; ++i) {
        QJniObject e = env->GetObjectArrayElement(o.object<jobjectArray>(), i);
        result.push_back(e.toString());
    }

    return result;
}


void jSetAuthorized(JNIEnv *env, jobject thiz, jlong cppPointer, jboolean v)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    auto cppAdapter = reinterpret_cast<Moqt::SpeechRecognizerAdapter*>(cppPointer);
    cppAdapter->setAuthorized(v);
}


void jPartialResultsReady(JNIEnv *env, jobject thiz, jlong cppPointer, jobject joTexts)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    auto cppAdapter = reinterpret_cast<Moqt::SpeechRecognizerAdapter*>(cppPointer);
    const QStringList texts = jobjectToQStringList(joTexts);
    qDebug() << "partialResultsReady" << texts;
    cppAdapter->partialResultsReady(texts);
}


void jResultsReady(JNIEnv *env, jobject thiz, jlong cppPointer, jobject joTexts)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    auto cppAdapter = reinterpret_cast<Moqt::SpeechRecognizerAdapter*>(cppPointer);
    const QStringList texts = jobjectToQStringList(joTexts);
    qDebug() << "resultsReady" << texts;
    cppAdapter->resultsReady(texts);
}


void jError(JNIEnv *env, jobject thiz, jlong cppPointer)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    auto cppAdapter = reinterpret_cast<Moqt::SpeechRecognizerAdapter*>(cppPointer);
    cppAdapter->error();
}


void registerNatveMethods()
{
    static bool registered;
    if (!registered) {
        const JNINativeMethod methods[] = {
                        {"setAuthorized", "(JZ)V", reinterpret_cast<void *>(jSetAuthorized)},
                        {"partialResultsReady", "(J[Ljava/lang/String;)V", reinterpret_cast<void *>(jPartialResultsReady)},
                        {"resultsReady", "(J[Ljava/lang/String;)V", reinterpret_cast<void *>(jResultsReady)},
                        {"error", "(J)V", reinterpret_cast<void *>(jError)}
                     };
        QJniEnvironment env;
        env.registerNativeMethods("org/moqt/speech/SpeechRecognizerAdapter", methods, sizeof(methods) / sizeof(methods[0]));
        registered = true;
    }
}

}


namespace Moqt
{


class SpeechRecognizerAndroidAdapter : public SpeechRecognizerAdapter
{
public:
    explicit SpeechRecognizerAndroidAdapter(SpeechRecognizer* parent) : SpeechRecognizerAdapter(parent),
                m_javaAdapter("org/moqt/speech/SpeechRecognizerAdapter",
                    "(JLjava/lang/String;I)V",
                    reinterpret_cast<jlong>(this),
                    QJniObject::fromString(parent->language()).object<jstring>(),
                    parent->maxResults())
    {
        registerNatveMethods();

        setAvailability(available(), availableOnDevice());
    }

    ~SpeechRecognizerAndroidAdapter() override
    {
        m_javaAdapter.callMethod<void>("destroy");
    }

    void requestAuthorization() override
    {
        const bool onDevice = speechRecognizer()->preferOnDeviceRecognition() && availableOnDevice();
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([javaAdapter = m_javaAdapter, onDevice] () {
            javaAdapter.callMethod<void>("requestAuthorization", "(Landroid/content/Context;Z)V",
                                         QNativeInterface::QAndroidApplication::context(), onDevice);
        });
    }

    void startListening(const QStringList& expectedPhrasesRef) override
    {
        const QStringList expectedPhrases = expectedPhrasesRef;
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([javaAdapter = m_javaAdapter, expectedPhrases] () {
            QJniEnvironment env;
            jclass jStringCls = env->FindClass("java/lang/String");
            jobjectArray jStrArray = env->NewObjectArray(static_cast<jsize>(expectedPhrases.size()), jStringCls, nullptr);
            for (int i = 0; i < expectedPhrases.size(); ++i) {
                const auto jphrase = QJniObject::fromString(expectedPhrases[i]);
                env->SetObjectArrayElement(jStrArray, i, jphrase.object());
            }
            javaAdapter.callMethod<void>("startListening", "([Ljava/lang/String;)V", jStrArray);
        });
    }

    void stopListening() override
    {
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([javaAdapter = m_javaAdapter] () {
            javaAdapter.callMethod<void>("stopListening");
        });
    }

private:
    QJniObject m_javaAdapter;

    bool available() const
    {
        return m_javaAdapter.callMethod<jboolean>("available", "(Landroid/content/Context;)Z", QNativeInterface::QAndroidApplication::context());
    }

    bool availableOnDevice() const
    {
        return m_javaAdapter.callMethod<jboolean>("availableOnDevice", "(Landroid/content/Context;)Z", QNativeInterface::QAndroidApplication::context());
    }
};


void SpeechRecognizer::initAdapter()
{
    m_adapter = new SpeechRecognizerAndroidAdapter(this);
}


}
