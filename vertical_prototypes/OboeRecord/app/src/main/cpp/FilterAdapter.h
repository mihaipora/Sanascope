#ifndef OBOERECORD_FILTER_H
#define OBOERECORD_FILTER_H

#include <cstdint>
#include "filt.h"

class FilterAdapter {
private:
    int32_t inputFreq;
    int32_t outputFreq;
    Filter* filter;
public:
    FilterAdapter(int32_t inputFreq, int32_t outputFreq);
    int16_t processFrame(int16_t frame);
};


#endif //OBOERECORD_FILTER_H
