#include "Stream.h"

Stream::Stream() : Loggable("SAudioStream") {
    infoLog("Creating stream...");
    streamBuilder -> setSharingMode(Stream::sharingMode)
            -> setChannelCount(Stream::channelCount)
            -> setFormat(Stream::format)
            -> setSampleRate(Stream::samplingRate)
            -> setPerformanceMode(Stream::performanceMode);
    infoLog("Stream created.");
}

void Stream::start() {
    infoLog("Starting stream...");
    if (stream) {
        oboe::Result result = stream->requestStart();
        if (result != oboe::Result::OK) {
            errorLog("Error starting stream: ", oboe::convertToText(result), "!");
        }
    } else {
        errorLog("stream is NULL!");
    }
}

void  Stream::stop() {
    infoLog("Stopping stream...");
    if (stream) {
        oboe::Result result = stream->requestStop();
        if (result != oboe::Result::OK) {
            errorLog("Error stopping Stream", oboe::convertToText(result), "!");
        }
    } else {
        errorLog("Stream is NULL!");
    }
}