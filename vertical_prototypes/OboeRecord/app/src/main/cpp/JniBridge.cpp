#include <jni.h>
#include "AudioEngine.h"

AudioEngine* ac = nullptr;
Loggable* logger = new Loggable("SJniBridge");

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_initialize(JNIEnv *env, jobject /* this */) {
    logger->infoLog("Initializing...");
    if (!ac) {
        ac = new AudioEngine();
    }
    logger->infoLog("Initialization finished.");
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_startRecording(JNIEnv *env, jobject /* this */) {
    logger->infoLog("Starting Recording...");
    if (!ac) {
        logger->errorLog("No AudioEngine object!");
    }
    ac->startRecording();
    logger->infoLog("Recording started.");
}
extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_stopRecording(JNIEnv *env, jobject /* this */) {
    logger->infoLog("Stopping Recording...");
    if (!ac) {
        logger->errorLog("No AudioEngine object!");
    }
    ac->stopRecording();
    logger->infoLog("Recording stopped.");
}
extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_startPlaying(JNIEnv *env, jobject /* this */) {
    logger->infoLog("Starting replay...");
    if (!ac) {
        logger->errorLog("No AudioEngine object!");
    }
    ac->startPlaying();
    logger->infoLog("Replay started.");
}
extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_stopPlaying(JNIEnv *env, jobject /* this */) {
    logger->infoLog("Stopping replay...");
    if (!ac) {
        logger->errorLog("No AudioEngine object!");
    }
    ac->stopPlaying();
    logger->infoLog("Replay stopped.");
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_setAmplification(JNIEnv *env, jobject /* this */,
                                                            jint newValue) {
    if (!ac) {
        logger->errorLog("No AudioEngine object.");
    }
    ac->setAmplificationFactor(newValue);
    logger->infoLog("Amplification factor changed.");
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_storeRecord(JNIEnv *env, jobject /* this */,
                                                         jstring jFilepath) {
    //path
    const char* path = env->GetStringUTFChars(jFilepath, 0);
    env->ReleaseStringUTFChars(jFilepath, path);
    std::string filepath = std::string(path);
    if (!ac) {
        logger->errorLog("No AudioEngine object.");
    }
    ac->storeRecord(filepath);
}