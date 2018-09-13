#ifndef OBOERECORD_DEBUGUTILS_H
#define OBOERECORD_DEBUGUTILS_H

#include <android/log.h>
#include <string>
#include <sstream>
#include <cstdarg>
#include <typeinfo>

class Loggable {
private:
    std::string logTag;

    template <typename T>
    std::string concat(T fstVal) const {
        std::stringstream ss;
        ss << fstVal;
        return ss.str();
    }
    template <typename T, typename... Arguments>
    std::string concat(T fstVal,Arguments... args) const {
        std::stringstream ss;
        ss << fstVal << concat(args...);
        return ss.str();
    }

public:
    Loggable(std::string logTag){
        this->logTag = logTag;
    };

    template<typename... Arguments> void infoLog(Arguments... args) const {
        const char * msg = concat(args...).c_str();
        __android_log_print(ANDROID_LOG_INFO, logTag.c_str(), "%s", msg);
    };

    template<typename... Arguments> void errorLog(Arguments... args) const {
        const char * msg = concat(args...).c_str();
        __android_log_print(ANDROID_LOG_ERROR, logTag.c_str(), "%s", msg);
    };
};

#endif //OBOERECORD_DEBUGUTILS_H
