#include "InStream.h"

InStream::InStream() {
    infoLog("Creating inStream...");
    oboe::Result result;

    /*oboe::AudioStreamBuilder* streamBuilder;

    streamBuilder -> setSharingMode(oboe::SharingMode::Exclusive)
            -> setChannelCount(Stream::channelCount)
            -> setFormat(Stream::format)
            -> setSampleRate(Stream::samplingRate);*/

    streamBuilder -> setDirection(oboe::Direction::Input)
            -> setCallback(nullptr);
    Loggable::infoLog("Opening inStream...");
    result = streamBuilder->openStream(&stream);
    if (result != oboe::Result::OK){
        Loggable::errorLog("Error opening inStream: ", oboe::convertToText(result), "!");
    }
    Loggable::infoLog("Input stream created successfully.");
}

oboe::ResultWithValue<int32_t> InStream::read(void *audioData, int32_t numFrames) {
    return stream->read(audioData, numFrames, 0);
}