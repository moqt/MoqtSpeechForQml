package org.moqt.speech;

//import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.speech.tts.TextToSpeech;
import android.speech.tts.UtteranceProgressListener;
import android.util.Log;
import java.util.Locale;


public class TextToSpeechAdapter {

    public TextToSpeechAdapter(Context context,
                               final long cppPointer,
                               final String language,
                               final float pitch,
                               final float rate,
                               final float volume,
                               final String voiceId) { // TODO voice is not implemented
        Log.d(TAG, "Initializing...");
        m_volume = volume;

        m_tts = new TextToSpeech(context, new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int status) {
                if (status == TextToSpeech.ERROR) {
                    Log.e(TAG, "Initialization error");
                } else {
                    if (!language.isEmpty()) {
                        m_tts.setLanguage(new Locale(language));
                    }
                    m_tts.setPitch(pitch);
                    m_tts.setSpeechRate(rate);
                    Log.d(TAG, "Initialization complete");
                    setInitialized(cppPointer, true);
                }
            }
        });

        m_tts.setOnUtteranceProgressListener(new UtteranceProgressListener() {
            // Called when an utterance "starts" as perceived by the caller
            @Override
            public void onStart(String utteranceId) {
            }
            // Called when an utterance has been stopped while in progress or flushed from the synthesis queue.
            @Override
            public void onStop(String utteranceId, boolean interrupted) {
                if (interrupted) {
                    speakFinished(cppPointer, utteranceId);
                }
            }
            // Called when an utterance has successfully completed processing.
            @Override
            public void onDone(String utteranceId) {
                speakFinished(cppPointer, utteranceId);
            }
            // Called when an error has occurred during processing.
            @Override
            public void onError(String utteranceId, int errorCode) {
                Log.e(TAG, "Processing error: " + utteranceId + ", code: " + errorCode);
            }
            @Override
            public void onError(String utteranceId) {
            }
        });
    }

    public void shutdown() {
        m_tts.shutdown();
    }

    public void pause() {
        m_tts.stop(); // There is no pause method
    }

    public void resume() {
    }

    public void speak(String text, String utteranceId) {
        Log.d(TAG, "TextToSpeechAdapter.speak");
        Bundle params = new Bundle();
        params.putFloat(TextToSpeech.Engine.KEY_PARAM_VOLUME, m_volume);
        if (m_tts.speak(text, TextToSpeech.QUEUE_FLUSH, params, utteranceId) != TextToSpeech.SUCCESS) {
            Log.e(TAG, "Speak error: " + utteranceId + ", text: " + text);
        }
    }

    public void playSilence(long duration, String utteranceId) {
        Log.d(TAG, "TextToSpeechAdapter.playSilence");
        if (m_tts.playSilentUtterance(duration, TextToSpeech.QUEUE_FLUSH, utteranceId) != TextToSpeech.SUCCESS) {
            Log.e(TAG, "Play silence error: " + utteranceId);
        }
    }

    private final float m_volume;
    private final TextToSpeech m_tts;

    private static final String TAG = "TextToSpeechAdapter";


    native private static void speakFinished(long cppPointer, String utteranceId);
    native private static void setInitialized(long cppPointer, boolean v);
}
