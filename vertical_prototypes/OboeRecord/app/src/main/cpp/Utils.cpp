#include "DebugUtils.h"
#include "RecordingParameters.h"

void debug::infoLog(const char* tag, const char* msg) {
    __android_log_print(ANDROID_LOG_INFO, tag, "%s", msg);
}

void debug::errorLog(const char* tag, const char* msg) {
    __android_log_print(ANDROID_LOG_ERROR, tag, "%s", msg);
}