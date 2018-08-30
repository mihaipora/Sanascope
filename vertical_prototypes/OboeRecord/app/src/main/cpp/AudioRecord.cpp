#include <cstring>
#include <cstdlib>
#include "AudioRecord.h"

AudioRecord::AudioRecord(uint32_t size) {
    recordingSize = size;
    binaryData = malloc(size);
    clear();
    reset();
}

void AudioRecord::clear() {
    memset(binaryData, 0, recordingSize);
}

void AudioRecord::reset() {
    recordingHead = 0;
    replayHead = 0;
}

void AudioRecord::appendFrames() {
    // TODO Implement
}

void AudioRecord::fetchFrames() {
    // TODO Implement
}
