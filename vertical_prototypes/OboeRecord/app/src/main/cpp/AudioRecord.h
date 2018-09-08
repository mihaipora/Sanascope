#ifndef OBOERECORD_AUDIORECORD_H
#define OBOERECORD_AUDIORECORD_H


#include <cstddef>
#include <cstdint>
#include "Loggable.h"


class AudioRecord : Loggable {
private:
    int16_t* frames;
    uint32_t maxSize = 0;

    uint32_t recordingHead = 0;
    uint32_t replayHead = 0;

public:
    AudioRecord(uint32_t size);
    void appendFrames(int16_t* frames, int32_t numFrames);
    bool fetchFrames(int16_t* frames, int32_t numFrames);
    void resetRecord();
    void resetReplay();
};


#endif //OBOERECORD_AUDIORECORD_H
