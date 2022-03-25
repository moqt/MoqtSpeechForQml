import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.moqt.speech

Window {
    id: window

    width: 640
    height: 480
    visible: true
    title: qsTr("MoqtSpeechForQml Example")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: window.height * 0.03

        Button {
            id: buttonSpeak
            text: "Speak"
            enabled: textArea.text.length > 0 && !textToSpeech.speaking
            Layout.alignment: Qt.AlignCenter
            Layout.minimumWidth: window.width * 0.4
            Layout.topMargin: window.height * 0.05
            onClicked: {
                textToSpeech.speak(textArea.text, () => {
                    console.debug("textToSpeech.speak() finsihed")
                })
            }
        }

        Button {
            id: buttonListen
            text: "Listen"
            enabled: speechRecognizer.available
            checkable: true
            Layout.alignment: Qt.AlignCenter
            Layout.minimumWidth: window.width * 0.4
            Layout.bottomMargin: window.height * 0.05
            onToggled: {
                if (speechRecognizer.listening) {
                    speechRecognizer.stopListening()
                } else {
                    const expectedPhrases = ["Hello!"] // Optional parameter
                    speechRecognizer.startListening(expectedPhrases)
                }
            }
            Binding {
                target: buttonListen
                property: "checked"
                value: speechRecognizer.listening
            }
        }

        TextArea {
            id: textArea
            Layout.margins: window.width * 0.05
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Hello speech world!"
        }
    }

    SpeechRecognizer {
        id: speechRecognizer

        language: "en-GB"
        Component.onCompleted: {
            // Request authorization before start of recognition
            speechRecognizer.requestAuthorization()
        }
        onAvailabilityChanged: {
            console.debug("Speech recognizer availability:", speechRecognizer.available, speechRecognizer.availableOnDevice)
        }
        onError: {
            console.error("Speech recognizer error!")
        }

        onPartialResultsReady: (texts) => {
            textArea.text = texts[0] ?? ""
        }
        onResultsReady: (texts) => {
            textArea.text = texts[0] ?? ""
        }
    }

    TextToSpeech {
        id: textToSpeech

        language: "en-GB"
    }
}
