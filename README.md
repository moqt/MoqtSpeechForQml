# MoqtSpeechForQml
Speech recognition and text to speech QML APIs 


## Android
Create android templates in QtCreator "Projects"

### build.gradle
Add to dependencies:
implementation "androidx.core:core-ktx:+"

Add to java.srcDirs path to 'MoqtSpeechForQml/sources/android/java'

### gradle.properties
android.useAndroidX=true

### AndroidManifest.xml
Add after </application>:

<queries>
    <intent>
        <action android:name="android.intent.action.TTS_SERVICE"/>
    </intent>
    <intent>
        <action android:name="android.speech.RecognitionService"/>
    </intent>
</queries>

<uses-permission android:name="android.permission.RECORD_AUDIO"/>


