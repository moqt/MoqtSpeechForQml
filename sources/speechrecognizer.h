#ifndef SPEECHRECOGNIZER_H
#define SPEECHRECOGNIZER_H

#include <QObject>
#include <QQmlParserStatus>
#include <QtQml/qqmlregistration.h>

namespace Moqt
{

class SpeechRecognizerAdapter;


class SpeechRecognizer : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(bool available READ available NOTIFY availabilityChanged FINAL)
    Q_PROPERTY(bool availableOnDevice READ availableOnDevice NOTIFY availabilityChanged FINAL)
    Q_PROPERTY(bool authorized READ authorized NOTIFY authorizedChanged FINAL)
    Q_PROPERTY(bool listening READ listening NOTIFY listeningChanged FINAL)
    Q_PROPERTY(QString language MEMBER m_language NOTIFY languageChanged FINAL) // For example "en-GB"
    Q_PROPERTY(int maxResults MEMBER m_maxResults NOTIFY maxResultsChanged FINAL)
    Q_PROPERTY(bool preferOnDeviceRecognition MEMBER m_preferOnDeviceRecognition NOTIFY preferOnDeviceRecognitionChanged FINAL)
    // TODO list of available languages

    Q_DISABLE_COPY_MOVE(SpeechRecognizer)

public:
    explicit SpeechRecognizer(QObject *parent = nullptr);

    bool available() const;
    bool availableOnDevice() const;
    bool authorized() const;
    bool listening() const;
    const QString& language() const;
    int maxResults() const;
    bool preferOnDeviceRecognition() const;

    void classBegin() override;
    void componentComplete() override;

public slots:
    void requestAuthorization();
    void startListening(const QStringList& expectedPhrases = QStringList()); // expectedPhrases is a hint parameter
    void stopListening();

signals:
    void availabilityChanged();
    void listeningChanged();
    void languageChanged();
    void maxResultsChanged();
    void preferOnDeviceRecognitionChanged();
    void authorizedChanged();
    void error();
    void partialResultsReady(QStringList texts); // An array of potential transcriptions, sorted in descending order of confidence.
    void resultsReady(QStringList texts);  // An array of potential transcriptions, sorted in descending order of confidence.

private slots:
    void setAuthorized(bool v);
    void setListening(bool v);
    void setAvailability(bool available, bool availableOnDevice);

private:
    SpeechRecognizerAdapter* m_adapter = nullptr;
    QString m_language;
    int m_maxResults = 5;
    bool m_available = false;
    bool m_availableOnDevice = false;
    bool m_authorized = false;
    bool m_listening = false;
    bool m_preferOnDeviceRecognition = true;

    void initAdapter();
};


}

#endif // SPEECHRECOGNIZER_H
