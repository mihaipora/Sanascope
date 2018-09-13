package com.sanascope.cstoragetest;

import android.media.MediaScannerConnection;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.io.FileWriter;

import static android.os.Environment.getExternalStorageState;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    final String LOG_TAG ="CStorage"; // tag to find the logcat output

    // Message to display if not all permissions were granted
    final String noPermission = "Storage permission denied";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (isExternalStorageWritable()){
            File jFile = new File(getPublicStorageDir("storageTest"), "java_test.txt");
            Log.i(LOG_TAG, jFile.getAbsolutePath());
            writeTestFile(jFile);

            File cFile = new File(getPublicStorageDir("storageTest"), "c_test.txt");
            Log.i(LOG_TAG, cFile.getAbsolutePath());
            writeFile(cFile.getAbsolutePath(), "Hello from C++.");

            MediaScannerConnection.scanFile(this, new String[] {jFile.toString()}, null, null);
            MediaScannerConnection.scanFile(this, new String[] {cFile.toString()}, null, null);
        } else {
            Log.e(LOG_TAG, "Storage not writable!");
        }

    }

    public boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state);
    }

    public File getPublicStorageDir(String albumName) {
        // Get the directory for the user's public pictures directory.
        File file = new File(Environment.getExternalStorageDirectory(), albumName);
        if (!file.mkdirs()) {
            Log.e(LOG_TAG, "Directory not created");
        }
        return file;
    }

    private void writeTestFile(File file){
        String textToWrite = "Hello from Java.";
        try {
            file.createNewFile();
            FileWriter writer = new FileWriter(file);
            writer.append(textToWrite);
            writer.flush();
            writer.close();
            Log.i(LOG_TAG, "Test file written");
            MediaScannerConnection.scanFile(this, new String[] {file.toString()}, null, null);
        } catch (Exception e) {
            //e.printStackTrace();
            Log.e(LOG_TAG, e.getMessage());
        }
    }

    private void listFiles(File dir){
        File[] listOfFiles = dir.listFiles();
        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile()) {
                Log.i(LOG_TAG, "File " + listOfFiles[i].getName());
            } else if (listOfFiles[i].isDirectory()) {
                Log.i(LOG_TAG, "Directory " + listOfFiles[i].getName());
            }
        }
    }

    /**
     * Writes creates a test file at in the folder given as jpath and writes jcontent into it.
     * @param jpath the folder where the test file should be created
     * @param jcontent the content to be written into the test file
     * @return a string summarizing the content and path
     */
    public native String writeFile(String jpath, String jcontent);
}
