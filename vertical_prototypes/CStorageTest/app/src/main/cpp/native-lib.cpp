#include <jni.h>
#include <string>
#include <iostream>
#include <fstream>
#include <android/native_activity.h>

extern "C" JNIEXPORT jstring
JNICALL
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

    //writing
    std::ofstream outfile (path_str + "/test.txt");
    outfile << content << std::endl;
    outfile.close();

    return env->NewStringUTF(("written " + cont_str + "\nto " + path_str).c_str());
}
