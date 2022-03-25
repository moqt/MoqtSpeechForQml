package org.moqt.speech;

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.util.Log;
import java.util.Arrays;
import java.util.Locale;
import android.speech.SpeechRecognizer;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.content.Intent;
import android.content.pm.PackageManager;
import androidx.core.app.ActivityCompat;


public class SpeechRecognizerAdapter {

    public SpeechRecognizerAdapter(final long cppPointer,
                                   final String language,
                                   final int maxResults) {
        m_cppPointer = cppPointer;
        m_language = language;
        m_maxResults = maxResults;
        Log.d(TAG, "Initialized");
    }

    public void destroy() {
        if (m_speechRecognizer != null) {
            m_speechRecognizer.destroy();
            m_speechRecognizer = null;
        }
    }

    public boolean available(Context context) {
        return SpeechRecognizer.isRecognitionAvailable(context);
    }

    public boolean availableOnDevice(Context context) {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S
            && SpeechRecognizer.isOnDeviceRecognitionAvailable(context);
    }

    public void requestAuthorization(Context context, boolean onDevice) {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M
            && context.checkSelfPermission(android.Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions((Activity) context, new String[] {android.Manifest.permission.RECORD_AUDIO}, 0);
        }

        if (onDevice) {
            m_speechRecognizer = SpeechRecognizer.createOnDeviceSpeechRecognizer(context);
        } else {
            m_speechRecognizer = SpeechRecognizer.createSpeechRecognizer(context);
        }

        if (m_speechRecognizer == null) {
            Log.e(TAG, "Could not create speech recognizer");
            error(m_cppPointer);
            return;
        }

        m_speechRecognizer.setRecognitionListener(new RecognitionListener() {
            @Override
            public void onReadyForSpeech(Bundle bundle) {
                Log.d(TAG, "onReadyForSpeech");
                m_active = true;
            }
            @Override
            public void onBeginningOfSpeech() {
                Log.d(TAG, "onBeginningOfSpeech");
            }
            @Override
            public void onRmsChanged(float v) {
            }
            @Override
            public void onBufferReceived(byte[] bytes) {
            }
            @Override
            public void onEndOfSpeech() {
                Log.d(TAG, "onEndOfSpeech");
                m_active = false;
            }
            @Override
            public void onError(int i) {
                Log.e(TAG, "onError: " + i);
                m_active = false;
                if (i != SpeechRecognizer.ERROR_NO_MATCH) { // This error happens on stopListening() call
                    error(m_cppPointer);
                }
            }
            @Override
            public void onResults(Bundle bundle) {
                final String[] texts = convertRecognitionResults(bundle, "onResults: ");
                resultsReady(m_cppPointer, texts);
            }
            @Override
            public void onPartialResults(Bundle bundle) {
                final String[] texts = convertRecognitionResults(bundle, "onPartialResults: ");
                partialResultsReady(m_cppPointer, texts);
            }
            @Override
            public void onEvent(int i, Bundle bundle) {
            }

            private String[] convertRecognitionResults(Bundle bundle, String dbgMessage) {
                // What’s with the weird-looking argument new String[0]? The reason it is there because the type of returned array is determined using this argument.
                final String[] texts = bundle.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION).toArray(new String[0]);
                final float[] scores = bundle.getFloatArray(SpeechRecognizer.CONFIDENCE_SCORES);
                Log.d(TAG, dbgMessage + Arrays.toString(scores));
                return texts;
            }
        });

        setAuthorized(m_cppPointer, true);
    }

    public void startListening(String[] expectedPhrases) {
        if (m_speechRecognizer != null) {
            Log.d(TAG, "SpeechRecognizer.startListening, lang: " + m_language);
            final Intent speechRecognizerIntent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
            speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
            if (!m_language.isEmpty()) {
                speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, m_language);
            }
            speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_MAX_RESULTS, m_maxResults);
            m_speechRecognizer.startListening(speechRecognizerIntent);

        } else {
            Log.e(TAG, "Speech recognizer is not authorized, call requestAuthorization()");
            error(m_cppPointer);
        }
    }

    public void stopListening() {
        if (m_speechRecognizer != null && m_active) {
            m_speechRecognizer.stopListening();
        }
    }


    private final long m_cppPointer;
    private final String m_language;
    private final int m_maxResults;
    private SpeechRecognizer m_speechRecognizer;
    private boolean m_active;

    private static final String TAG = "SpeechRecognizerAdapter";


    native private static void setAuthorized(long cppPointer, boolean v);
    native private static void partialResultsReady(long cppPointer, String[] texts);
    native private static void resultsReady(long cppPointer, String[] texts);
    native private static void error(long cppPointer);
}
