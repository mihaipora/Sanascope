#ifndef OBOERECORD_AUDIORECORDER_H
#define OBOERECORD_AUDIORECORDER_H

#include <oboe/Oboe.h>
#include "RecordingParameters.h"
#include "AudioRecord.h"
#include "DebugUtils.h"


class AudioRecorder : public oboe::AudioStreamCallback{
private:
    const char* LOG_TAG = "SAudioRecorder";

    uint64_t processedFrameCount = 0;

    AudioRecord* rec;
    oboe::AudioStream* recStream;
    oboe::AudioStream* playStream;

public:
    /*
     * oboe::AudioStreamCallback interface implementation
     */
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

    AudioRecorder(AudioRecord* arec);
    void startRecording();
    void stopRecording();

private:
    void startStream(oboe::AudioStream *stream);
    void stopStream(oboe::AudioStream *stream);
};


#endif //OBOERECORD_AUDIORECORDER_H
