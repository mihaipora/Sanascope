#include <jni.h>
#include <string>
#include <iostream>
#include <fstream>
#include <android/log.h>

/**
 * Logs a message in the info-stream
 * @param msg The message to log
 */
void infoLog(const char* msg) {
    __android_log_print(ANDROID_LOG_INFO, "CStoragePP", "%s", msg);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_sanascope_cstoragetest_MainActivity_writeFile(
        JNIEnv *env,
        jobject /* this */,
        jstring jpath,
        jstring jcontent) {

    //path
    const char* path = env->GetStringUTFChars(jpath, 0);
    env->ReleaseStringUTFChars(jpath, path);
    const std::string path_str = std::string(path);

    //content
    const char* content = env->GetStringUTFChars(jcontent, 0);
    env->ReleaseStringUTFChars(jcontent, content);
    const std::string cont_str = std::string(content);

    // create and open file stream
    std::ofstream outfile(path_str); // opens by default an output stream
    if (outfile.is_open()) {
        infoLog("filestream open");
    } else {
        infoLog("filestream not open");
    }

    // write the file and close the stream
    outfile << content << std::endl;
    outfile.close();

    return env->NewStringUTF(("written " + cont_str + "\nto " + path_str).c_str());
}
