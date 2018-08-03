#include <jni.h>
#include <string>
#include <oboe/Oboe.h>
#include <android/log.h>

/**
 * Logs the result in case of error
 *
 * @param result The result to be checked
 * @param errorMessage The message to log before the result string
 */
void checkResult(oboe::Result result, const char* errorMessage) {
    if (result != oboe::Result::OK){
        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", errorMessage, convertToText(result));
    }
}

/**
 * Imports oboe and creates, starts, stops and closes an input audio stream
 */
extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_recordAudio(
        JNIEnv *env,
        jobject /* this */) {

    // Initialize the stream builder
    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Input)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive);
    // Keep the defaults for everything else

    // Open stream and ensure it was successfully opened
    oboe::AudioStream *stream;
    oboe::Result result = builder.openStream(&stream);
    checkResult(result, "Error opening stream %s");
    // result is: ErrorInternal
    // Possibilities:
    // 1. The stream is not correctly initialized
    // 2. Something went wrong with the permission

    /*
    result = stream->requestStart();
    checkResult(result, "Error starting stream %s");
    result = stream->requestStop();
    checkResult(result, "Error stopping stream %s");
    result = stream->close();
    checkResult(result, "Error closing stream %s");
     */
}
