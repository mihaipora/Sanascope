package com.sanascope.cstoragetest;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    final String logTag ="MainActivityLog"; // tag to find the logcat output

    // Message to display if not all permissions were granted
    final String noPermission = "Storage permission denied";
    int PERMISSION_ALL = 1;
    TextView tv; // The only text field in MainActivity (for outputs on the device)

    final String fileContent = "Content of test file";
    String filePath;

    /**
     * To be tested:
     * Creating a file from C++ code in internal or external
     * Verifying that the file exists
     *
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv = (TextView) findViewById(R.id.sample_text);

//        // check whether the SD card is mounted
//        String storageState = Environment.getExternalStorageState();
//        if (!Environment.MEDIA_MOUNTED.equals(storageState)){
//            tv.setText("SD card not mounted");
//            return;
//        }

        // Path to write the new file into
        //filePath = getFilesDir().getAbsolutePath(); // internal
        filePath = Environment.getExternalStorageDirectory().getAbsolutePath(); // external

        // make sure the permission is granted this very moment (seems mandatory!?)
        while (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            requestStoragePermission();
            try {
                Thread.sleep(2000);
            } catch (Exception e) {
                Log.e(logTag, e.getMessage());
            }
        }
        // if the permission was granted before, the file is written for the first time here
        // if not it should have been written already written when the permission was granted
        // and is overwritten here (Yeah ugly I know. I'll think of something better)
        writeFile(filePath, fileContent);

        // list files from filepath in infolog
        Log.i(logTag, "listing files...");
        File directory = new File(filePath);
        File[] files = directory.listFiles();
        if (files != null) {
            for (File f : files) {
                Log.i(logTag, f.getName());
            }
        } else {
            Log.i(logTag, "no files written");
        }
    }

    /**
     * Requests explicit permission to read and write external storage
     * and triggers onRequestPermissionsResult()
     */
    private void requestStoragePermission(){
        Log.i(logTag, "permission request sent");
        String[] PERMISSIONS = {
                android.Manifest.permission.WRITE_EXTERNAL_STORAGE,
                android.Manifest.permission.READ_EXTERNAL_STORAGE
        };
        ActivityCompat.requestPermissions(this, PERMISSIONS, PERMISSION_ALL);
    }

    /**
     * permission check from stack overflow
     */
//    public static boolean hasPermissions(Context context, String... permissions) {
//        if (context != null && permissions != null) {
//            for (String permission : permissions) {
//                if (ActivityCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
//                    return false;
//                }
//            }
//        }
//        return true;
//    }

    /**
     * calls writeFile() if the permission is granted,
     * otherwise prints error message in main activity
     *
     * @param requestCode
     * @param permissions
     * @param grantResults
     */
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        Log.i(logTag, "Permission request arrived");
        if (PERMISSION_ALL != requestCode) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            Log.i(logTag,"exiting onRequestPermissionResult()");
            return;
        }
        if (grantResults.length != 1 ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED) {

            // User denied the permission, without this we cannot record audio
            // Show a toast and update the status accordingly
            tv.setText(noPermission);
            Toast.makeText(getApplicationContext(), noPermission, Toast.LENGTH_SHORT).show();
        } else {
            // Permission was granted, create file
            Log.i(logTag, "creating file...");
            writeFile(filePath, fileContent);
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
