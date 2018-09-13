#include <cstring>
#include <cstdlib>
#include "AudioRecord.h"

AudioRecord::AudioRecord(uint32_t size) : Loggable("SAudioRecord") {
    infoLog("Initializing record object...");
    maxSize = size;
    frames = new int16_t[size/2];
    resetRecord();
    resetReplay();
    infoLog("Record object initialized.");
}

bool AudioRecord::isEmpty() const {
    return recordingHead > 0;
}

void AudioRecord::resetRecord() {
    memset(frames, 0, maxSize);
    recordingHead = 0;
    replayHead = 0;
}

void AudioRecord::resetReplay() {
    replayHead = 0;
}

/*
 * TODO Prevent overflows
 * TODO Consider using memcopy
 */
void AudioRecord::appendFrames(int16_t* frames, int32_t numFrames) {
    if (recordingHead + numFrames >= maxSize) {
        errorLog("Writing over the bounds!");
        return;
    }
    if (frames == nullptr) {
        errorLog("No space for the recording allocated!");
        return;
    }
    infoLog("Appending frames...");
    for (int32_t i = 0; i < numFrames; i++) {
        this->frames[recordingHead+i] = frames[i];
    }
    recordingHead += numFrames;
    infoLog("Appended ", numFrames," frames, recording head now at ", recordingHead);
}

/*
 * TODO Prevent overflows
 * @returns true if there is still more data to replay
 */
bool AudioRecord::fetchFrames(int16_t* frames, int32_t numFrames) {
    if (recordingHead <= 0) {
        errorLog("Trying to replay empty record.");
        return false;
    }
    for (int32_t i = 0; i < numFrames; i++) {
        frames[i] = this->frames[replayHead + i];
    }
    replayHead += numFrames;
    infoLog("Fetched ", numFrames," frames, replay head now at ", replayHead, ".");

    bool moreData = replayHead + numFrames < recordingHead;
    return moreData;
}

void AudioRecord::writeFile(std::string filepath) const {
    // create and open file stream
    std::ofstream outfile(filepath + "/testfile.txt"); // opens by default an output stream
    if (outfile.is_open()) {
        infoLog("Filestream open");
    } else {
        infoLog("Filestream not open");
    }

    // write the file and close the stream
    outfile << "Surprise Motherfucker!!" << std::endl;
    outfile.close();
}