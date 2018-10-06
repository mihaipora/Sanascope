package com.sanascope.oboerecord;

import android.Manifest;
import android.content.pm.PackageManager;
import android.media.MediaScannerConnection;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Toast;

import java.io.File;
import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class MainActivity extends AppCompatActivity {

    final String logTag = "SAudioJMain";
    boolean permissionsGranted = false;
    final String noPermission = "Audio record or storage write permission denied";
    int AUDIO_EFFECT_REQUEST;
    EngineState state = EngineState.IDLE;
    Button recordButton;
    Button playButton;
    Button saveButton;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // record button
        recordButton = (Button) findViewById(R.id.RecordButton);
        recordButton.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v)
            {
                // Some Android versions require explicit requests for dangerous permissons
                if (!permissionsGranted) { requestRecordPermission(); }
                toggleRecord();
            }
        });

        // replay button
        playButton = (Button) findViewById(R.id.PlayButton);
        playButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if (!permissionsGranted) { requestRecordPermission(); }
                togglePlay(); }
        });

        // save button
        saveButton = (Button) findViewById(R.id.SaveButton);
        saveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!permissionsGranted) { requestRecordPermission(); }
                writeFile();
            }
        });

        // Seek bar
        SeekBar seekBar = (SeekBar) findViewById(R.id.amplificationBar);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if (b) {setAmplification(i);}
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        initialize();
    }

    private void togglePlay() {
        if (state == EngineState.IDLE) {
            recordButton.setEnabled(false);
            saveButton.setEnabled(false);
            state = EngineState.PLAYING;
            startPlaying();
        } else if (state == EngineState.PLAYING) {
            stopPlaying();
            state = EngineState.IDLE;
            recordButton.setEnabled(true);
            saveButton.setEnabled(true);
        } else {
            Log.e(logTag, "Tapped play at an illegal state!");
        }
    }

    private void toggleRecord() {
        if (state == EngineState.IDLE) {
            playButton.setEnabled(false);
            saveButton.setEnabled(false);
            state = EngineState.RECODING;
            startRecording();
        } else if (state == EngineState.RECODING) {
            stopRecording();
            state = EngineState.IDLE;
            playButton.setEnabled(true);
            saveButton.setEnabled(true);
        } else {
            Log.e(logTag, "Tapped record at an illegal state!");
        }
    }

    private void writeFile() {
        SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
        String dateString = formatter.format(new Date());

        String folderName = getResources().getString(R.string.record_folder_name);
        String fileName = dateString + ".wav";
        File folderPath, filePath;

        // external storage for now to allow for easy transfer
        folderPath = new File(Environment.getExternalStorageDirectory()
                + File.separator + folderName);

        // create folder if it doesn't exist yet
        if (!folderPath.exists()) {
            if (!folderPath.mkdirs()) {
                Toast.makeText(this,
                        getResources().getString(R.string.error_msg_folder_creation),
                        Toast.LENGTH_SHORT).show();
                return;
            }
        }

        filePath = new File(folderPath, fileName);

        String filePathStr = filePath.getAbsolutePath();
        storeRecord(filePathStr); // make it bool and display dependant toast
        Toast.makeText(this, "saved as " + fileName, Toast.LENGTH_SHORT).show();

        // workaround to refresh the file browser
        MediaScannerConnection.scanFile(this, new String[] {filePathStr}, null, null);
    }

    /**
     * Requests explicit permission for audio record and triggers onRequestPermissionsResult()
     */
    private void requestRecordPermission(){
        ActivityCompat.requestPermissions(
                this,
                new String[]{Manifest.permission.RECORD_AUDIO,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE},
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

        if (grantResults.length != 2 ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED ||
                grantResults[1] != PackageManager.PERMISSION_GRANTED) {

            // User denied the permission, without this we cannot record audio
            // Show a toast and update the status accordingly
            Toast.makeText(getApplicationContext(),
                    noPermission,
                    Toast.LENGTH_SHORT)
                    .show();
        } else {
            permissionsGranted = true;
        }
    }

    /**
     * Imports oboe and creates and starts an input audio stream,
     * records some frames, then stops and closes the stream.
     */
    public native void startRecording();
    public native void stopRecording();
    public native void startPlaying();
    public native void stopPlaying();
    public native void initialize();
    public native void setAmplification(int factor);
    public native void storeRecord(String filepath);
}
