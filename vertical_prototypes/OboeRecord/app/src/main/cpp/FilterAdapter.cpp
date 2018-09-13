#include "FilterAdapter.h"

FilterAdapter::FilterAdapter(int32_t inputFreq, int32_t outputFreq) {
    this->inputFreq = inputFreq;
    this->outputFreq = outputFreq;
    filter = new Filter(LPF, 51, inputFreq/1000.0, outputFreq/1000.0);
}

int16_t FilterAdapter::processFrame(int16_t frame) {
    //return filter->do_sample(frame/32767.0)*32767;
    return frame;
}