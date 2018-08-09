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
 * Logs a message in the info-stream
 * @param msg The message to log
 */
void infoLog(const char* msg) {
    __android_log_print(ANDROID_LOG_INFO, "AudioEngine", "%s", msg);
}

void* initializeBuffer(){
    int16_t *audioData = new int16_t[1000]();
    return audioData;
}

/**
 * Creates and starts an audio stream, records some frames, then stops and closes the stream
 */
extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_recordAudio(
        JNIEnv *env,
        jobject /* this */) {

    // reacts to the incoming frames
    class InputCallback : public oboe::AudioStreamCallback {
    public:
        oboe::DataCallbackResult
        onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames){
            infoLog("recording...");
            audioData = initializeBuffer();
            return oboe::DataCallbackResult::Continue;
        }
    };

    // reacts to the outgoing frames
    class OutputCallback : public oboe::AudioStreamCallback {
    public:
        oboe::DataCallbackResult
        onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames){
            infoLog("playing...");
            return oboe::DataCallbackResult::Stop;
        }
    };

    InputCallback inputCallback;

    // Initialize the stream builder
    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Input)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setFormat(oboe::AudioFormat::I16)
            //->setBufferCapacityInFrames(100)
            ->setCallback(&inputCallback);
    // Keep the defaults for everything else

    // Open stream and ensure it was successfully opened
    infoLog("opening stream...");
    oboe::AudioStream *stream;
    oboe::Result result = builder.openStream(&stream);
    checkResult(result, "Error opening stream %s");

    // Start stream if it was successfully created
    if (stream) {
        infoLog("starting stream...");
        result = stream->requestStart();
        checkResult(result, "Error starting stream %s");
    }


    // Record one second of audio data
    int sr = stream->getSampleRate();
    int cc = stream->getChannelCount();

    infoLog("starting record...");
    void* audioData = initializeBuffer();
    int numFrames = sr;
    // App crashes here
    // Possible causes
    //   1. can't write in audioData
    //   2. Buffer overflows
    result = stream->read(audioData, numFrames, 0);
    checkResult(result, "Error reading stream %s");

    // Stop the stream
    infoLog("stopping stream...");
    result = stream->requestStop();
    checkResult(result, "Error stopping stream %s");

    // Close the stream
    infoLog("closing stream...");
    result = stream->close();
    checkResult(result, "Error closing stream %s");
    infoLog("stream closed...");

    while (true) {
        infoLog("terminated");
        sleep(2);
    }
}
