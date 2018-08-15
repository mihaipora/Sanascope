#include <jni.h>
#include <string>
#include <oboe/Oboe.h>
#include <android/log.h>
#include <sstream>

void infoLog(const char* msg) {
    __android_log_print(ANDROID_LOG_INFO, "AudioEngine", "%s", msg);
}

void errorLog(const char* msg) {
    __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "%s", msg);
}

class AudioThroughput : public oboe::AudioStreamCallback {
private:
    bool isOn = false;
    bool fromMemory = false;

    int32_t mSampleRate = 0; // is later set to the default recording sample rate.
    uint64_t mProcessedFrameCount = 0;
    uint64_t mReplayedFrameCount = 0;
    uint64_t mSystemStartupFrames = 0;
    int32_t mInputChannelCount = oboe::ChannelCount::Stereo;
    int32_t mOutputChannelCount = oboe::ChannelCount::Stereo;
    oboe::AudioStream *mRecordingStream = nullptr;
    oboe::AudioStream *mPlayStream = nullptr;

    const size_t bytesPerFrame = 2;
    const size_t recsize = 480000;
    char recording[480000];

private:
    const float kSystemWarmupTime = 0.5f;

    // causes segmentation fault
    void stopStream(oboe::AudioStream *stream) {
        if (stream) {
            infoLog("stopping stream...");
            oboe::Result result = stream->stop(0L);
            infoLog("stream stopped");
            if (result != oboe::Result::OK) {
                errorLog("Error stopping stream");
            }
        }
    };

    void closeStream(oboe::AudioStream *stream) {
        if (stream) {
            oboe::Result result = stream->close();
            if (result != oboe::Result::OK) {
                errorLog("Error closing stream. %s");
            }
        }
    };

    void closeAndStopAll(){
        infoLog("closing all streams...");
        //stopStream(mPlayStream);
        //stopStream(mRecordingStream);
        closeStream(mPlayStream);
        closeStream(mRecordingStream);
        infoLog("all streams closed");
    };

    oboe::AudioStreamBuilder* setupCommonStreamParameters(
            oboe::AudioStreamBuilder *builder) {
        builder ->setSharingMode(oboe::SharingMode::Exclusive)
                ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
                ->setFormat(oboe::AudioFormat::I16);
        return builder;
    }

    oboe::AudioStreamBuilder* setupRecordingStreamParameters(
            oboe::AudioStreamBuilder *builder) {
        assert(mSampleRate != 0);
        builder ->setCallback(nullptr)
                ->setDirection(oboe::Direction::Input)
                ->setChannelCount(mInputChannelCount)
                ->setSampleRate(mSampleRate);
        return setupCommonStreamParameters(builder);
    }

    oboe::AudioStreamBuilder* setupPlaybackStreamParameters(
            oboe::AudioStreamBuilder *builder) {
        builder ->setCallback(this)
                ->setDirection(oboe::Direction::Output)
                ->setChannelCount(mOutputChannelCount);
        return setupCommonStreamParameters(builder);
    }

    void openRecordingStream() {
        oboe::AudioStreamBuilder builder;
        setupRecordingStreamParameters(&builder);
        oboe::Result result = builder.openStream(&mRecordingStream);
        if (result == oboe::Result::OK && mRecordingStream) {
            assert(mRecordingStream->getChannelCount() == mInputChannelCount);
            assert(mRecordingStream->getFormat() == oboe::AudioFormat::I16);
        } else {
            errorLog("Failed to create recording stream");
        }
    }

    void openPlaybackStream() {
        oboe::AudioStreamBuilder builder;
        setupPlaybackStreamParameters(&builder);
        oboe::Result result = builder.openStream(&mPlayStream);
        if (result == oboe::Result::OK && mPlayStream) {
            mSampleRate = mPlayStream->getSampleRate();
            assert(mPlayStream->getFormat() == oboe::AudioFormat::I16);
            assert(mOutputChannelCount == mPlayStream->getChannelCount());
            mSystemStartupFrames = static_cast<uint64_t>(mSampleRate * kSystemWarmupTime);
            mProcessedFrameCount = 0;
        } else {
            errorLog("Failed to create playback stream.");
        }
    }

    void startStream(oboe::AudioStream *stream) {
        assert(stream);
        if (stream) {
            oboe::Result result = stream->requestStart();
            if (result != oboe::Result::OK) {
                errorLog("Error starting stream.");
            }
        }
    }

    void openAllStreams() {
        infoLog("opening streams...");
        if (fromMemory) {
            openPlaybackStream();
            if (mRecordingStream && mPlayStream) {
                startStream(mPlayStream);
            } else {
                errorLog("Failed to create replay stream");
                closeAndStopAll();
            }
        } else {
            openPlaybackStream();
            openRecordingStream();
            if (mRecordingStream && mPlayStream) {
                startStream(mRecordingStream);
                startStream(mPlayStream);
            } else {
                errorLog("Failed to create recording and/or playback stream");
                closeAndStopAll();
            }
        }
    }

public:
    AudioThroughput() {
        assert(mOutputChannelCount == mInputChannelCount);
        memset(recording, 0, recsize);
        infoLog("object created");
    };
    ~AudioThroughput(){
        closeAndStopAll();
        infoLog("object deleted");
    };

    void loadRecordedSamples(void* buffer, size_t frameOffset, size_t numFrames) {
        memcpy(buffer, &recording[frameOffset*bytesPerFrame], numFrames*bytesPerFrame);
    }

    int32_t getSamplRate() const {
        return mSampleRate;
    }

    int64_t getProcessedFrameCount(){
        return mProcessedFrameCount;
    }

    /*
     * oboe::AudioStreamCallback interface implementation
     */
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error){
        errorLog("error before closing stream");
    };
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error){
        errorLog("error after closing stream");
    };
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames){
        if (fromMemory) {
            infoLog("replaying from memory...");
            if (mReplayedFrameCount < mProcessedFrameCount) {
                infoLog("Loading additional frames to replay...");
                loadRecordedSamples(oboeStream, mReplayedFrameCount, numFrames);
                std::ostringstream oss;
                oss << "Replayed " << mReplayedFrameCount << " frames out of " << mProcessedFrameCount;
                infoLog((new std::string(oss.str()))->c_str());
                mReplayedFrameCount += numFrames;
                infoLog("Continuing replay...");
                return oboe::DataCallbackResult::Continue;
            } else {
                infoLog("replay ended");
                return oboe::DataCallbackResult::Stop;
            }
        } else {
            assert(oboeStream == mPlayStream);
            int32_t prevFrameRead = 0, framesRead = 0;
            if (mProcessedFrameCount < mSystemStartupFrames) {
                do {
                    prevFrameRead = framesRead;
                    oboe::ResultWithValue<int32_t> status =
                            mRecordingStream->read(audioData, numFrames, 0);
                    framesRead = (!status) ? 0 : status.value();
                    if (framesRead == 0) break;
                } while (framesRead);

                framesRead = prevFrameRead;
            } else {
                oboe::ResultWithValue<int32_t> status =
                        mRecordingStream->read(audioData, numFrames, 0);
                if (!status) {
                    errorLog("input stream read error");
                    return oboe::DataCallbackResult::Stop;
                }
                framesRead = status.value();
            }

            if (framesRead < numFrames) {
                int32_t bytesPerFrame = mRecordingStream->getChannelCount() *
                                        oboeStream->getBytesPerSample();
                uint8_t *padPos =
                        static_cast<uint8_t *>(audioData) + framesRead * bytesPerFrame;
                memset(padPos, 0, (size_t) (numFrames - framesRead) * bytesPerFrame);
            }

            // add your audio processing here
            memcpy(&recording[mProcessedFrameCount], audioData, numFrames * bytesPerFrame);

            mProcessedFrameCount += numFrames;

            std::ostringstream oss;
            oss << mProcessedFrameCount;
            infoLog((new std::string(oss.str()))->c_str());

            if (mProcessedFrameCount < recsize) {
                return oboe::DataCallbackResult::Continue;
            } else {
                return oboe::DataCallbackResult::Stop;
            }
        }
    };

    void toggleThroughput(){
        infoLog("toggled throughput...");
        fromMemory = false;
        closeAndStopAll();
        isOn = !isOn;
        if (isOn) {
            openAllStreams();
        } else {
            closeAndStopAll();
        }
    }

    void toggleRecordReplay(){
        infoLog("toggled throughput...");
        fromMemory = !fromMemory;
        mReplayedFrameCount = 0;
        isOn = false;
        closeAndStopAll();
        if (fromMemory) {
            openAllStreams();
        } else {
            closeAndStopAll();
        }
    }
};


AudioThroughput* at = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_initialize(
        JNIEnv *env,
        jobject /* this */) {
    infoLog("initializing");
    at = new AudioThroughput();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_throughput(
        JNIEnv *env,
        jobject /* this */) {
    infoLog("throughput toggled");
    if (!at){
        errorLog("recreating the AudioThroughput object");
        at = new AudioThroughput();
    }
    at->toggleThroughput();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sanascope_oboerecord_MainActivity_replay(
        JNIEnv *env,
        jobject /* this */) {
    infoLog("replay toggled");
    if (!at){
        errorLog("recreating the AudioThroughput object");
        at = new AudioThroughput();
    }
    at->toggleRecordReplay();
}

