#include "AudioRecorder.h"

AudioRecorder::AudioRecorder(AudioRecord* arec) {
    rec = arec;
    oboe::AudioStreamBuilder* streamBuilder;
    oboe::Result result;

    // replay stream
    streamBuilder -> setDirection(oboe::Direction::Output)
            -> setSharingMode(oboe::SharingMode::Exclusive)
            -> setChannelCount(sanastream::CHANNEL_COUNT)
            -> setFormat(sanastream::AUDIO_FORMAT)
            -> setSampleRate(sanastream::SAMPLING_RATE)
            -> setCallback(this);
    debug::infoLog(LOG_TAG, "Opening playStream...");
    result = streamBuilder->openStream(&playStream);
    if (result != oboe::Result::OK){
        debug::errorLog(LOG_TAG, oboe::convertToText(result));
    }
    debug::infoLog(LOG_TAG, "Throughput stream created successfully.");


    // recording stream
    streamBuilder -> setDirection(oboe::Direction::Input)
            -> setCallback(nullptr);
    debug::infoLog(LOG_TAG, "Opening recStream...");
    result = streamBuilder->openStream(&recStream);
    if (result != oboe::Result::OK){
        debug::errorLog(LOG_TAG, oboe::convertToText(result));
    }
    debug::infoLog(LOG_TAG, "Recording stream created successfully.");
}

void AudioRecorder::startRecording(){
    debug::infoLog(LOG_TAG, "Starting recStream...");
    startStream(recStream);
    debug::infoLog(LOG_TAG, "recStream started.");
    debug::infoLog(LOG_TAG, "Starting playStream...");
    startStream(playStream);
    debug::infoLog(LOG_TAG, "playStream started.");
}

void AudioRecorder::stopRecording(){
    debug::infoLog(LOG_TAG, "Stopping playStream...");
    stopStream(playStream);
    debug::infoLog(LOG_TAG, "playStream stopped.");
    debug::infoLog(LOG_TAG, "Stopping recStream...");
    stopStream(recStream);
    debug::infoLog(LOG_TAG, "recStream stopped.");
}

void AudioRecorder::startStream(oboe::AudioStream *stream){
    if (stream) {
        oboe::Result result = stream->requestStart();
        if (result != oboe::Result::OK) {
            debug::errorLog(LOG_TAG, oboe::convertToText(result));
        }
    } else {
        debug::errorLog(LOG_TAG, "stream is NULL");
    }
}

void AudioRecorder::stopStream(oboe::AudioStream *stream){
    debug::infoLog(LOG_TAG, "Stopping stream...");
    if (stream) {
        oboe::Result result = stream->requestStop();
        if (result != oboe::Result::OK) {
            debug::errorLog(LOG_TAG, oboe::convertToText(result));
        }
    } else {
        debug::errorLog(LOG_TAG, "stream is NULL");
    }
}

void AudioRecorder::setAmplificationFactor(int factor) {
    amplificationFactor = factor;
}

oboe::DataCallbackResult AudioRecorder::onAudioReady(oboe::AudioStream *audioStream,
                                                     void *audioData, int32_t numFrames){
    int32_t prevFrameRead = 0, framesRead = 0;
    if (processedFrameCount < sanastream::WARMUP_FRAMES) {
        do {
            // Drain the audio for the starting up period, half second for
            // this sample.
            prevFrameRead = framesRead;

            oboe::ResultWithValue<int32_t> status = recStream->read(audioData, numFrames, 0);
            framesRead = (!status) ? 0 : status.value();
            if (framesRead == 0) break;

        } while (framesRead);

        framesRead = prevFrameRead;
    } else {
        oboe::ResultWithValue<int32_t> status = recStream->read(audioData, numFrames, 0);
        if (!status) {
            debug::errorLog(LOG_TAG, oboe::convertToText(status.error()));
            return oboe::DataCallbackResult ::Stop;
        }
        framesRead = status.value();
    }

    if (framesRead < numFrames) {
        int32_t bytesPerFrame = recStream->getChannelCount() * sanastream::FRAME_SIZE;
        uint8_t *padPos =
                static_cast<uint8_t *>(audioData) + framesRead * bytesPerFrame;
        memset(padPos, 0, (size_t)(numFrames - framesRead) * bytesPerFrame);
    }

    // audio processing
    debug::infoLog(LOG_TAG, "Casting audioData...");
    int16_t* data = static_cast<int16_t*>(audioData);
    for (int i = 0; i < numFrames; i++) {
        data[i] = amplificationFactor * data[i];
    }
    audioData = data;

    processedFrameCount += numFrames;

    return oboe::DataCallbackResult::Continue;
}

void AudioRecorder::onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) {
    debug::errorLog(LOG_TAG, oboe::convertToText(error));
}

void AudioRecorder::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
    debug::errorLog(LOG_TAG, oboe::convertToText(error));
}