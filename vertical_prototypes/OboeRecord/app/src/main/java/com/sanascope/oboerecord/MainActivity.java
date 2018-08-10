package com.sanascope.oboerecord;

import android.Manifest;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    final String noPermission = "Audio record permission denied";
    int AUDIO_EFFECT_REQUEST;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button toggleButton = (Button) findViewById(R.id.toggle_button);
        toggleButton.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v)
            {
                // Some Android versions require explicit requests for dangerous permissons
                requestRecordPermission();
            }
        });
        initialize();
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
            Toast.makeText(getApplicationContext(),
                    noPermission,
                    Toast.LENGTH_SHORT)
                    .show();
        } else {
            // Permission was granted, start recording
            throughput();
        }
    }

    /**
     * Imports oboe and creates and starts an input audio stream,
     * records some frames, then stops and closes the stream.
     */
    public native void throughput();
    public native void initialize();
}
