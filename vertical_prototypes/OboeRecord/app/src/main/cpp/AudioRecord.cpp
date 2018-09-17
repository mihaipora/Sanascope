#include <cstring>
#include <cstdlib>
#include "AudioRecord.h"
#include "Stream.h"

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

bool AudioRecord::writeFile(std::string filepath) const {
    if (isEmpty()) {
        errorLog("Trying to store empty record.");
        return false;
    }

    // create and open file stream
    std::ofstream* outfile = new std::ofstream(); // opens by default an output stream
    outfile->open(filepath, std::ofstream::out);
    if (outfile->is_open()) {
        infoLog("Filestream open.");
        if (!writeHeader(outfile)){
            errorLog("Header couldn't be written!");
            return false; // no need for more precise information than "writing failed"
        }
        infoLog("Header written.");
        if (!writeFrames(outfile)){
            errorLog("Frames couldn't be written!");
            return false;
        }
        infoLog("Frames written.");
    } else {
        errorLog("Filestream not open.");
        return false;
    }
    outfile->close();
    return true;
}

void writeBytes(uint8_t* bytes, uint8_t numBytes, std::ofstream* targetStream){
    for (int i=0; i<numBytes; i++) {
        *targetStream << bytes[i];
    }
}

/*
bool isLittleEndian () {
    uint32_t num = 1;
    return *(uint16_t *)&num == 1;
}

template <typename I>
I littleEndian(I i){
    if (isLittleEndian()) {
        return i;
    } else {
        return i;
    }
}*/

bool AudioRecord::writeHeader(std::ofstream* targetStream) const {
    infoLog("Writing WAV header.");

    //big_int32_at x = 100;

    /*
    uint8_t* RIFF_HEADER = new uint8_t[] { 0x52, 0x49, 0x46, 0x46 };
    uint8_t* FORMAT_WAVE = new uint8_t[] { 0x57, 0x41, 0x56, 0x45 };
    uint8_t* FORMAT_TAG  = new uint8_t[] { 0x66, 0x6d, 0x74, 0x20 };
    uint8_t* AUDIO_FORMAT = new uint8_t[] {0x01, 0x00};
    uint8_t* SUBCHUNK_ID  = new uint8_t[] { 0x64, 0x61, 0x74, 0x61 };

    const uint8_t headerSize = 36;
    const uint64_t dataSize = recordingHead*Stream::bytesPerSample*Stream::channelCount;
    */
    return true;
}

bool AudioRecord::writeFrames(std::ofstream* fileStream) const {
    infoLog("Writing WAV frames.");
    return true;
}
