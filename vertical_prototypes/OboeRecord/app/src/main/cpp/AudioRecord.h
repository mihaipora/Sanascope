#ifndef OBOERECORD_AUDIORECORD_H
#define OBOERECORD_AUDIORECORD_H


#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "Loggable.h"


class AudioRecord : Loggable {
private:
    int16_t* frames;
    uint32_t maxSize = 0;

    uint32_t recordingHead = 0;
    uint32_t replayHead = 0;

public:
    AudioRecord(uint32_t size);
    bool isEmpty() const;
    void appendFrames(int16_t* frames, int32_t numFrames);
    bool fetchFrames(int16_t* frames, int32_t numFrames);
    void resetRecord();
    void resetReplay();
    void writeFile(std::string filepath) const;
};


#endif //OBOERECORD_AUDIORECORD_H
