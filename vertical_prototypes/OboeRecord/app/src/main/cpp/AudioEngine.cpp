#include <jni.h>
#include "AudioRecord.h"
#include "Loggable.h"
#include "AudioEngine.h"


AudioEngine::AudioEngine() : Loggable("SAudioEngine") {
    infoLog("Creating audio engine...");
    state = EngineState::Idle;
    record = new AudioRecord(Stream::maxRecordSize);
    filter = new FilterAdapter(Stream::samplingRate, Stream::samplingRate/12);
    inStream = new InStream();
    outStream = new OutStream(this);
    setAmplificationFactor(10);
    infoLog("Audio engine created successfully.");
}

void AudioEngine::startRecording() {
    if (state != EngineState::Idle) {
        errorLog("Trying to start recording from a state other than Idle!");
        return;
    }
    infoLog("Starting recording...");
    state = EngineState::Recording;
    record->resetRecord();
    record->resetReplay();
    inStream->start();
    outStream->start();
    infoLog("Recording started.");
}

void AudioEngine::stopRecording() {
    if (state != EngineState::Recording) {
        errorLog("Trying to stop recording from a state other than Recording!");
        return;
    }
    infoLog("Stopping recording...");
    outStream->stop();
    inStream->stop();
    state = EngineState::Idle;
    infoLog("Recording stopped.");
}

void AudioEngine::startPlaying() {
    if (state != EngineState::Idle) {
        errorLog("Trying to start playing from a state other than Idle!");
        return;
    }
    infoLog("Starting replay...");
    state = EngineState::Playing;
    record->resetReplay();
    outStream->start();
    infoLog("Replay started.");
}

void AudioEngine::stopPlaying() {
    if (state != EngineState::Playing) {
        errorLog("Trying to stop playing from a state other than Playing!");
        return;
    }
    infoLog("Stopping replay...");
    outStream->stop();
    state = EngineState::Idle;
    infoLog("Replay stopped.");
}


oboe::DataCallbackResult
AudioEngine::onAudioReady(oboe::AudioStream* oboeStream, void* audioData, int32_t numFrames) {
    if (state == EngineState::Recording){
        infoLog("Recording callback requested...");
        auto icb = inputCallback(audioData, numFrames);
        infoLog("recording callback delivered.");
        return icb;
    } else if (state == EngineState::Playing) {
        infoLog("Replay callback requested...");
        return outputCallback(audioData, numFrames);
    } else {
        errorLog("Callback requested during state Idle!");
        return oboe::DataCallbackResult::Stop;
    }
}

void AudioEngine::onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) {
    AudioStreamCallback::onErrorBeforeClose(oboeStream, error);
    errorLog("Error before closing stream: ", oboe::convertToText(error));
}

void AudioEngine::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
    AudioStreamCallback::onErrorAfterClose(oboeStream, error);
    errorLog("Error after closing stream: ",  oboe::convertToText(error));
}

oboe::DataCallbackResult AudioEngine::inputCallback(void* audioData, int32_t numFrames) {
    int32_t prevFrameRead = 0, framesRead = 0;
    if (processedFrameCount < Stream::warmupFrames) {
        infoLog("Warmup padding up to ", Stream::warmupFrames, " frames...");
        do {
            // Drain the audio for the starting up period, half second for
            // this sample.
            prevFrameRead = framesRead;

            oboe::ResultWithValue<int32_t> status = inStream->read(audioData, numFrames);
            framesRead = (!status) ? 0 : status.value();
            if (framesRead == 0) break;

        } while (framesRead);
        infoLog("Padding complete.");

        framesRead = prevFrameRead;
    } else {
        infoLog("No padding required. Reading from input stream...");
        oboe::ResultWithValue<int32_t> status = inStream->read(audioData, numFrames);
        if (!status) {
            errorLog("Error reading audio data: ", oboe::convertToText(status.error()));
            return oboe::DataCallbackResult::Stop;
        }
        framesRead = status.value();
        infoLog(framesRead, "additional frames read.");
    }

    if (framesRead < numFrames) {
        infoLog("Reading additional frames to fill the throughput buffer...");
        int32_t bytesPerFrame = Stream::channelCount * Stream::bytesPerSample;
        uint8_t *padPos =
                static_cast<uint8_t *>(audioData) + framesRead * bytesPerFrame;
        memset(padPos, 0, (size_t)(numFrames - framesRead) * bytesPerFrame);
    }
    infoLog("Throughput buffer filled.");

    // audio processing
    infoLog("Amplifying audioData...");
    int16_t* data = static_cast<int16_t*>(audioData);
    for (int i = 0; i < numFrames; i++) {
        data[i] = filter->processFrame(ampliFactor * data[i]);
    }
    infoLog("Audio data amplified.");
    audioData = data;

    // storing the processed audio
    infoLog("Writing audio data into RAM...");
    record->appendFrames(data, numFrames);
    infoLog("Audio data written.");


    processedFrameCount += numFrames;

    return oboe::DataCallbackResult::Continue;
}

oboe::DataCallbackResult AudioEngine::outputCallback(void *audioData, int32_t numFrames) {
    bool keepPlaying = record->fetchFrames(static_cast<int16_t *>(audioData), numFrames);
    if (keepPlaying) {
        return oboe::DataCallbackResult::Continue;
    } else {
        return oboe::DataCallbackResult::Stop;
    }
}