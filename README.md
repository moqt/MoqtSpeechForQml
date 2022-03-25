# MoqtSpeechForQml
Speech recognition and text to speech QML APIs.

***Tested with Qt 6.2.3***

### Backends
- Android
- iOS
- Mock


## Examples

### Text to speech:
```qml
TextToSpeech {
    id: textToSpeech
    language: "en-GB"
}

Button {
    text: "Speak"
    onClicked: {
        textToSpeech.speak("Hello world!", () => {
            console.debug("textToSpeech.speak() finsihed")
        })
    }
}
```

### Speech recognizer:
```qml
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
        console.debug("onPartialResultsReady:", texts[0])
    }
    onResultsReady: (texts) => {
        console.debug("onResultsReady:", texts[0])
    }
}

Button {
    id: buttonListen
    text: "Listen"
    enabled: speechRecognizer.available
    checkable: true
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
```



## Android
Create android templates in QtCreator "Projects" of your app.

### build.gradle
Add to dependencies:
```
implementation "androidx.core:core-ktx:+"
```

Add to java.srcDirs path to 'MoqtSpeechForQml/sources/android/java', for instance:
```
java.srcDirs = [qtAndroidDir + '/src', 'src', 'java', '../../../sources/android/java']
```


### gradle.properties
```
android.useAndroidX=true
```

### AndroidManifest.xml
Add after </application>:
```
<queries>
    <intent>
        <action android:name="android.intent.action.TTS_SERVICE"/>
    </intent>
    <intent>
        <action android:name="android.speech.RecognitionService"/>
    </intent>
</queries>

<uses-permission android:name="android.permission.RECORD_AUDIO"/>
```

## iOS
Link Speech framework:
```
LIBS += -framework Speech
LIBS += -framework AVFoundation
```

### Info.plist
Add usage descriptions:
```
<key>NSMicrophoneUsageDescription</key>
<string>This app uses your microphone to recognize speech</string>
<key>NSSpeechRecognitionUsageDescription</key>
<string>This app uses Speech recognition to transform your spoken words into text</string>
```
