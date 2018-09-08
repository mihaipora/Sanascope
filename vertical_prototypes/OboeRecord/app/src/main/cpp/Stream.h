#ifndef OBOERECORD_STREAM_H
#define OBOERECORD_STREAM_H


#include <oboe/Oboe.h>
#include "Loggable.h"

class Stream : public Loggable {
public:
    static uint32_t const maxRecordSize = 20000000;
    static oboe::AudioFormat const format = oboe::AudioFormat::I16;
    static oboe::ChannelCount const channelCount = oboe::ChannelCount::Mono;
    static uint32_t const samplingRate = 8000;
    static uint8_t const bytesPerSample = 2;
    static uint16_t const warmupFrames = 22000;
    static oboe::PerformanceMode const performanceMode = oboe::PerformanceMode::None;
    static oboe::SharingMode const sharingMode = oboe::SharingMode::Exclusive;

protected:
    oboe::AudioStreamBuilder* streamBuilder = new oboe::AudioStreamBuilder();
    oboe::AudioStream* stream = nullptr;

    Stream();

public:
    void start();
    void stop();
};


#endif //OBOERECORD_STREAM_H
