#ifndef OBOERECORD_AUDIOENGINE_H
#define OBOERECORD_AUDIOENGINE_H

#include "AudioRecord.h"
#include "InStream.h"
#include "OutStream.h"
#include "FilterAdapter.h"

enum class EngineState {Idle, Recording, Playing};

class AudioEngine : Loggable, oboe::AudioStreamCallback {
private:
    AudioRecord* record;
    EngineState state;
    int ampliFactor = 0;
    uint64_t processedFrameCount = 0;

    InStream* inStream;
    OutStream* outStream;
    FilterAdapter* filter;

    oboe::DataCallbackResult outputCallback(void* audioData, int32_t numFrames);
    oboe::DataCallbackResult inputCallback(void* audioData, int32_t numFrames);
public:
    AudioEngine();
    void startRecording();
    void stopRecording();
    void startPlaying();
    void stopPlaying();
    void setAmplificationFactor(int factor) {ampliFactor = factor; };

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) override;
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;
};

#endif //OBOERECORD_AUDIOENGINE_H
