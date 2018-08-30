#ifndef OBOERECORD_AUDIORECORD_H
#define OBOERECORD_AUDIORECORD_H


#include <cstddef>
#include <cstdint>
#include "DebugUtils.h"


class AudioRecord {
private:
    const char* LOG_TAG = "SAudioRecord";

    void* binaryData;
    uint32_t recordingSize = 0;

    uint32_t recordingHead = 0;
    uint32_t replayHead = 0;

public:
    AudioRecord(uint32_t size);
    void clear();
    void reset();
    void appendFrames();
    void fetchFrames();
};


#endif //OBOERECORD_AUDIORECORD_H
