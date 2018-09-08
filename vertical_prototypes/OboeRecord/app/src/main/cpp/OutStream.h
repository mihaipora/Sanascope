#ifndef OBOERECORD_OUTSTREAM_H
#define OBOERECORD_OUTSTREAM_H


#include "Stream.h"

class OutStream : public Stream {
public:
    OutStream(oboe::AudioStreamCallback* callback);
};


#endif //OBOERECORD_OUTSTREAM_H
