#include "OutStream.h"

OutStream::OutStream(oboe::AudioStreamCallback* callback) {
    infoLog("Creating OutStream...");

    /*oboe::AudioStreamBuilder* streamBuilder;

    streamBuilder -> setSharingMode(oboe::SharingMode::Exclusive)
            -> setChannelCount(Stream::channelCount)
            -> setFormat(Stream::format)
            -> setSampleRate(Stream::samplingRate);*/

    streamBuilder -> setDirection(oboe::Direction::Output)
            -> setCallback(callback);
    Loggable::infoLog("Opening outStream...");
    oboe::Result result = streamBuilder->openStream(&stream);
    if (result != oboe::Result::OK){
        Loggable::errorLog("Error opening outStream: ", oboe::convertToText(result));
    }
    Loggable::infoLog("Output stream created successfully.");
}