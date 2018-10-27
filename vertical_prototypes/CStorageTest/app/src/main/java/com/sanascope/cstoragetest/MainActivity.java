package com.sanascope.cstoragetest;

import android.media.MediaScannerConnection;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferUtility;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferState;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferListener;
import com.amazonaws.services.s3.AmazonS3Client;

import java.io.File;
import java.io.FileWriter;

import static android.os.Environment.getExternalStorageState;
import static android.provider.Contacts.SettingsColumns.KEY;
import static java.security.KeyRep.Type.SECRET;

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
        AWSMobileClient.getInstance().initialize(this).execute();
        setContentView(R.layout.activity_main);
        if (isExternalStorageWritable()){
            File jFile = new File(getPublicStorageDir("storageTest"), "java_test.txt");
            Log.i(LOG_TAG, jFile.getAbsolutePath());
            writeTestFile(jFile);

            uploadWithTransferUtility(jFile);

            File cFile = new File(getPublicStorageDir("storageTest"), "c_test.txt");
            Log.i(LOG_TAG, cFile.getAbsolutePath());
            writeFile(cFile.getAbsolutePath(), "Hello from C++.");

            uploadWithTransferUtility(cFile);

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

    public void uploadWithTransferUtility(File fileToUpload) {

        String fileName = fileToUpload.getName();
        String absolutePath = fileToUpload.getAbsolutePath();

        Log.i(LOG_TAG, "uploading file " + fileName + " from " + absolutePath);

        TransferUtility transferUtility =
                TransferUtility.builder()
                        .context(getApplicationContext())
                        .awsConfiguration(AWSMobileClient.getInstance().getConfiguration())
                        .s3Client(new AmazonS3Client(AWSMobileClient.getInstance().getCredentialsProvider()))
                        .build();

        TransferObserver uploadObserver =
                transferUtility.upload(
                        "public/" + fileName,
                        new File(absolutePath));

        // Attach a listener to the observer to get state update and progress notifications
        uploadObserver.setTransferListener(new TransferListener() {

            @Override
            public void onStateChanged(int id, TransferState state) {
                if (TransferState.COMPLETED == state) {
                    // Handle a completed upload.
                }
            }

            @Override
            public void onProgressChanged(int id, long bytesCurrent, long bytesTotal) {
                float percentDonef = ((float) bytesCurrent / (float) bytesTotal) * 100;
                int percentDone = (int)percentDonef;

                Log.d("YourActivity", "ID:" + id + " bytesCurrent: " + bytesCurrent
                        + " bytesTotal: " + bytesTotal + " " + percentDone + "%");
            }

            @Override
            public void onError(int id, Exception ex) {
                // Handle errors
            }

        });

        // If you prefer to poll for the data, instead of attaching a
        // listener, check for the state and progress in the observer.
        if (TransferState.COMPLETED == uploadObserver.getState()) {
            // Handle a completed upload.
        }

        Log.d("YourActivity", "Bytes Transferred: " + uploadObserver.getBytesTransferred());
        Log.d("YourActivity", "Bytes Total: " + uploadObserver.getBytesTotal());
    }

    /**
     * Writes creates a test file at in the folder given as jpath and writes jcontent into it.
     * @param jpath the folder where the test file should be created
     * @param jcontent the content to be written into the test file
     * @return a string summarizing the content and path
     */
    public native String writeFile(String jpath, String jcontent);
}
