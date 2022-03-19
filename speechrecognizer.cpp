#include "speechrecognizeradapter.h"

#include <QDebug>


namespace Moqt
{


SpeechRecognizerAdapter::SpeechRecognizerAdapter(SpeechRecognizer* parent)
    : QObject(parent), m_speechRecognizer(parent)
{
}


void SpeechRecognizerAdapter::setAuthorized(bool v)
{
    QMetaObject::invokeMethod(m_speechRecognizer, "setAuthorized", Qt::QueuedConnection,
                              Q_ARG(bool, v));
}


void SpeechRecognizerAdapter::partialResultsReady(QStringList texts)
{
    QMetaObject::invokeMethod(m_speechRecognizer, "partialResultsReady", Qt::QueuedConnection,
                              Q_ARG(QStringList, texts));

}


void SpeechRecognizerAdapter::resultsReady(QStringList texts)
{
    QMetaObject::invokeMethod(m_speechRecognizer, "resultsReady", Qt::QueuedConnection,
                              Q_ARG(QStringList, texts));
    QMetaObject::invokeMethod(m_speechRecognizer, "setListening", Qt::QueuedConnection,
                              Q_ARG(bool, false));
}


void SpeechRecognizerAdapter::error()
{
    QMetaObject::invokeMethod(m_speechRecognizer, "error", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_speechRecognizer, "setListening", Qt::QueuedConnection,
                              Q_ARG(bool, false));
}


void SpeechRecognizerAdapter::setAvailability(bool available, bool availableOnDevice)
{
    QMetaObject::invokeMethod(m_speechRecognizer, "setAvailability", Qt::QueuedConnection,
                              Q_ARG(bool, available), Q_ARG(bool, availableOnDevice));
}


SpeechRecognizer::SpeechRecognizer(QObject *parent)
    : QObject{parent}
{
}


void SpeechRecognizer::classBegin()
{
}


void SpeechRecognizer::componentComplete()
{
    Q_ASSERT(!m_adapter);
    initAdapter();
}


bool SpeechRecognizer::authorized() const
{
    return m_authorized;
}


void SpeechRecognizer::setAuthorized(bool v)
{
    if (v != m_authorized) {
        m_authorized = v;
        emit authorizedChanged();
    }
}


bool SpeechRecognizer::listening() const
{
    return m_listening;
}


void SpeechRecognizer::setListening( bool v)
{
    if (v != m_listening) {
        m_listening = v;
        emit listeningChanged();
    }
}


void SpeechRecognizer::setAvailability(bool available, bool availableOnDevice)
{
    m_available = available;
    m_availableOnDevice = availableOnDevice;
    qDebug() << "SpeechRecognizer::setAvailablity" << available << availableOnDevice;
    emit availabilityChanged();
}


bool SpeechRecognizer::available() const
{
    return m_available;
}


bool SpeechRecognizer::availableOnDevice() const
{
    return m_availableOnDevice;
}


void SpeechRecognizer::requestAuthorization()
{
    qDebug() << "SpeechRecognizer::requestAuthorization";

    if (m_adapter) {
        m_adapter->requestAuthorization();
    }
}


void SpeechRecognizer::startListening(const QStringList& expectedPhrases)
{
    qDebug() << "SpeechRecognizer::startListening, expectedPhrases:\n" << expectedPhrases;
    setListening(true);
    if (m_adapter) {
        m_adapter->startListening(expectedPhrases);
    }
}


void SpeechRecognizer::stopListening()
{
    qDebug() << "SpeechRecognizer::stopListening";
    if (m_adapter) {
        m_adapter->stopListening();
    }
    setListening(false);
}


const QString& SpeechRecognizer::language() const
{
    return m_language;
}


int SpeechRecognizer::maxResults() const
{
    return m_maxResults;
}


bool SpeechRecognizer::preferOnDeviceRecognition() const
{
    return m_preferOnDeviceRecognition;
}




}
