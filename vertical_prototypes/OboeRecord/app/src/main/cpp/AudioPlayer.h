#ifndef OBOERECORD_AUDIOPLAYER_H
#define OBOERECORD_AUDIOPLAYER_H

#include <oboe/AudioStream.h>
#include "AudioRecord.h"
#include "RecordingParameters.h"


class AudioPlayer {
private:
    const char* LOG_TAG = "SAudioPlayer";

    AudioRecord* rec;
    bool isPlaying = false;

public:

    AudioPlayer(AudioRecord* arec);
    void startPlaying();
    void stopPlaying();
};


#endif //OBOERECORD_AUDIOPLAYER_H
