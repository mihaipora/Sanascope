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
    return recordingHead <= 0;
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

template <typename T>
bool writeBinary(std::ofstream* targetStream, T data){
    char* binary = ((char*) &data);
    for (char i = 0; i < sizeof(T); i++) {
        *targetStream << binary[i];
    }
    return true;
}

bool AudioRecord::writeFile(std::string filepath) const {
    if (isEmpty()) {
        errorLog("Trying to store empty record.");
        return false;
    }

    // create and open file stream
    std::ofstream* outfile = new std::ofstream(); // opens by default an output stream
    outfile->open(filepath, std::ios::out|std::ios::binary|std::ios::ate);
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



bool AudioRecord::writeHeader(std::ofstream* targetStream) const {

    infoLog("Writing WAV header.");
    using namespace boost::endian;

    infoLog("Preparing header fields.");
    // RIFF chunk descriptor
    const big_int32_t RIFF_HEADER = 0x52494646;
    const little_int32_t headerSize = native_to_little(36);
    const little_int32_t dataSize = native_to_little(recordingHead * Stream::bytesPerSample * Stream::channelCount);
    const little_int32_t CHUNK_SIZE = native_to_little(headerSize+dataSize);
    const big_int32_t FORMAT_WAVE = 0x57415645;

    // fmt subchunk
    const big_int32_t SUBCHUNK_1_ID = 0x666d7420;
    const little_int32_t SUBCHUNK_1_SIZE = native_to_little(16);
    const little_int16_t AUDIO_FORMAT = native_to_little(1);
    const little_int16_t NUM_CHANNELS = native_to_little(Stream::channelCount);
    const little_int32_t SAMPLE_RATE = native_to_little(Stream::samplingRate);
    const little_int32_t BYTE_RATE = native_to_little(Stream::samplingRate * Stream::channelCount * Stream::bytesPerSample);
    const little_int16_t BLOCK_ALIGN = native_to_little(Stream::channelCount * Stream::bytesPerSample);
    const little_int16_t BITS_PER_SAMPLE = native_to_little(16);

    // data subchunk
    const big_int32_t SUBCHUNK_2_ID = 0x64617461;
    const little_int32_t SUBCHUNK_2_SIZE = native_to_little(dataSize);
    infoLog("Header fields prepared.");

    infoLog("Writing header fields.");
    writeBinary(targetStream, RIFF_HEADER);
    writeBinary(targetStream, CHUNK_SIZE);
    writeBinary(targetStream, FORMAT_WAVE);

    writeBinary(targetStream, SUBCHUNK_1_ID);
    writeBinary(targetStream, SUBCHUNK_1_SIZE);
    writeBinary(targetStream, AUDIO_FORMAT);
    writeBinary(targetStream, NUM_CHANNELS);
    writeBinary(targetStream, SAMPLE_RATE);
    writeBinary(targetStream, BYTE_RATE);
    writeBinary(targetStream, BLOCK_ALIGN);
    writeBinary(targetStream, BITS_PER_SAMPLE);

    writeBinary(targetStream, SUBCHUNK_2_ID);
    writeBinary(targetStream, SUBCHUNK_2_SIZE);
    infoLog("Header fields written.");

    return true;
}

bool AudioRecord::writeFrames(std::ofstream* fileStream) const {
    infoLog("Writing WAV frames.");
    for (uint32_t i = 0; i < recordingHead; i++){
        writeBinary(fileStream, boost::endian::native_to_little(frames[i]));
    }
    return true;
}
