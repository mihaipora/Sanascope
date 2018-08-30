#include <jni.h>
#include "AudioRecord.h"
#include "AudioRecorder.h"
#include "DebugUtils.h"
#include "RecordingParameters.h"

const char* LOG_TAG = "SAudioController";

class AudioController {
private:
    AudioRecord* record;
    AudioRecorder* recorder;

    bool throughputActive = false;

public:
    AudioController();
    void toggleThroughput();
    void setAmplificationFactor(int factor) {recorder->setAmplificationFactor(factor);};
};

AudioController::AudioController() {
    record = new AudioRecord(sanastream::MAX_RECORD_SIZE);
    recorder = new AudioRecorder(record);
    setAmplificationFactor(10);
    debug::infoLog(LOG_TAG, "Controller created successfully.");
}

void AudioController::toggleThroughput() {
    if (throughputActive) {
        throughputActive = false;
        recorder->stopRecording();
    } else {
        throughputActive = true;
        recorder->startRecording();
    }
}



AudioController* ac = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_initialize(
        JNIEnv *env,
jobject /* this */) {
    debug::infoLog(LOG_TAG, "Initializing...");
    if (!ac) {
        ac = new AudioController();
    }
    debug::infoLog(LOG_TAG, "Initialization finished");
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_throughput(
        JNIEnv *env,
jobject /* this */) {
    debug::infoLog(LOG_TAG, "throughput toggled");
    if (!ac) {
        debug::errorLog(LOG_TAG, "no AudioController object");
    }
    ac->toggleThroughput();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_setAmplification(JNIEnv *env,
                                                            jobject /* this */,
                                                            jint newValue) {
    if (!ac) {
        debug::errorLog(LOG_TAG, "no AudioController object");
    }
    ac->setAmplificationFactor(newValue);
    debug::infoLog(LOG_TAG, "amplification factor changed");
}