package com.sanascope.oboerecord;

import android.Manifest;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.security.Permission;

public class MainActivity extends AppCompatActivity {

    int MY_PERMISSIONS_REQUEST_RECORD_AUDIO;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Some Android versions require explicit requests for dangerous permissons
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.RECORD_AUDIO},
                MY_PERMISSIONS_REQUEST_RECORD_AUDIO);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        recordAudio();
    }

    /**
     * Imports oboe and creates, starts, stops and closes an input audio stream
     */
    public native void recordAudio();
}
