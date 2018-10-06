#ifndef OBOERECORD_AUDIORECORD_H
#define OBOERECORD_AUDIORECORD_H


#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <boost/endian/arithmetic.hpp>
#include <boost/endian/conversion.hpp>
#include <arpa/inet.h>
#include "Loggable.h"
#include "Endian.h"


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
    bool writeFile(std::string filepath) const;

private:
    bool writeHeader(std::ofstream* fileStream) const;
    bool writeFrames(std::ofstream* fileStream) const;
};


#endif //OBOERECORD_AUDIORECORD_H
