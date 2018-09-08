#ifndef OBOERECORD_INSTREAM_H
#define OBOERECORD_INSTREAM_H


#include "Stream.h"

class InStream : public Stream {
public:
    InStream();
    oboe::ResultWithValue<int32_t> read(void* audioData, int32_t numFrames);
};


#endif //OBOERECORD_INSTREAM_H
