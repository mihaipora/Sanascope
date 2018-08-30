#include <oboe/AudioStreamBuilder.h>
#include "AudioPlayer.h"


AudioPlayer::AudioPlayer(AudioRecord* arec) {
    rec = arec;
    oboe::AudioStreamBuilder* streamBuilder;
    streamBuilder -> setDirection(oboe::Direction::Input)
            -> setSharingMode(oboe::SharingMode::Exclusive)
            -> setChannelCount(sanastream::CHANNEL_COUNT)
            -> setFormat(sanastream::AUDIO_FORMAT)
            -> setCallback(nullptr);
}

