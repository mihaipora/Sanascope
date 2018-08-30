#ifndef OBOERECORD_RECORDINGPARAMETERS_H
#define OBOERECORD_RECORDINGPARAMETERS_H

#include <oboe/Oboe.h>

namespace sanastream {

    const uint32_t MAX_RECORD_SIZE = 20000000; // 20 MB

    const oboe::AudioFormat AUDIO_FORMAT = oboe::AudioFormat::I16;
    const oboe::ChannelCount CHANNEL_COUNT = oboe::ChannelCount::Mono;
    const int32_t SAMPLING_RATE = 8000;
    const uint8_t FRAME_SIZE = 2;

    const uint16_t WARMUP_FRAMES = 22000;
}

#endif //OBOERECORD_RECORDINGPARAMETERS_H
