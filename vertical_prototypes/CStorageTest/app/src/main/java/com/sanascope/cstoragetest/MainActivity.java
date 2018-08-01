package com.sanascope.cstoragetest;

import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * To be tested:
     * Creating a file from C++ code (ideally on SD card)
     * Verifying that the file exists
     *
     * Working:
     * Writing in internal storage and on SD card seems to work
     * Listing internal storage shows the newly created file
     *
     * Failing:
     * App crashes when trying to list files on SD card
     *
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView tv = (TextView) findViewById(R.id.sample_text);

        // check whether the SD card is mounted
        String storageState = Environment.getExternalStorageState();
        if (!Environment.MEDIA_MOUNTED.equals(storageState)){
            tv.setText("SD card not mounted");
            return;
        }

        // Path to write the new file into
        String filePath = getFilesDir().getAbsolutePath();
        //String filePath = Environment.getExternalStorageDirectory().getAbsolutePath();

        // call to the JNI
        String feedback = "foo";
        feedback = writeFile(filePath, "Testfile");

        // list files
        File directory = new File(filePath);
        File[] files = directory.listFiles();
        String files_str = "";
        for (File f : files) {
            files_str += f.getName() + ",\n";
        }

        // in case listing the files is commented out
        if (files_str=="") {
            files_str = directory.getAbsolutePath();
        }

        // writing to the text field in MainActivity
        tv.setText(feedback + "\n\nFiles:\n" + files_str);
    }

    /**
     * Writes creates a test file at in the folder given as jpath and writes jcontent into it.
     * @param jpath the folder where the test file should be created
     * @param jcontent the content to be written into the test file
     * @return a string summarizing the content and path
     */
    public native String writeFile(String jpath, String jcontent);
}
