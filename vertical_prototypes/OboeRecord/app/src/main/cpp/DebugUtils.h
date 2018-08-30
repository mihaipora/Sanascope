#ifndef OBOERECORD_DEBUGUTILS_H
#define OBOERECORD_DEBUGUTILS_H

#include <android/log.h>

namespace debug {
    void infoLog(const char* tag, const char* msg);
    void errorLog(const char* tag, const char* msg);
}

#endif //OBOERECORD_DEBUGUTILS_H
