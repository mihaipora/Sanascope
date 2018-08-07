package com.sanascope.oboerecord;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import java.security.Permission;

public class MainActivity extends AppCompatActivity {

    final String noPermission = "Audio record permission denied";
    int AUDIO_EFFECT_REQUEST;
    TextView tv;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv = (TextView) findViewById(R.id.sample_text);

        // Some Android versions require explicit requests for dangerous permissons
        requestRecordPermission();
    }

    /**
     * Requests explicit permission for audio record and triggers onRequestPermissionsResult()
     */
    private void requestRecordPermission(){


        ActivityCompat.requestPermissions(
                this,
                new String[]{Manifest.permission.RECORD_AUDIO},
                AUDIO_EFFECT_REQUEST);
    }

    /**
     * calls recordAudio() if the permission is granted,
     * otherwise prints error message in main activity
     *
     * @param requestCode
     * @param permissions
     * @param grantResults
     */
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        if (AUDIO_EFFECT_REQUEST != requestCode) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            return;
        }

        if (grantResults.length != 1 ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED) {

            // User denied the permission, without this we cannot record audio
            // Show a toast and update the status accordingly
            tv.setText(noPermission);
            Toast.makeText(getApplicationContext(),
                    noPermission,
                    Toast.LENGTH_SHORT)
                    .show();
        } else {
            // Permission was granted, start recording
            recordAudio();
        }
    }

    /**
     * Imports oboe and creates and starts an input audio stream,
     * records some frames, then stops and closes the stream.
     */
    public native void recordAudio();
}
