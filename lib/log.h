#pragma once

#include <iterator>
#include <string>
#include <stdio.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_HALT  3

#define LOG_TRACE Log(LOG_LEVEL_TRACE, __FILE__, __LINE__)
#define LOG_INFO  Log(LOG_LEVEL_INFO,  __FILE__, __LINE__)
#define LOG_WARN  Log(LOG_LEVEL_WARN,  __FILE__, __LINE__)

#define LOG_HALT  Log(LOG_LEVEL_HALT,  __FILE__, __LINE__, true)

namespace LogDetails {
    void output(std::string & msg, double val);
    void output(std::string & msg, int16_t val);
    void output(std::string & msg, int32_t val);
    void output(std::string & msg, int64_t val);
    void output(std::string & msg, uint16_t val);
    void output(std::string & msg, uint32_t val);
    void output(std::string & msg, uint64_t val);
    void output(std::string & msg, const std::string & val);

    template<typename T>
    inline void output(std::string & msg, const T& val) {
        output(msg, val.toString());
    }

    const char * filenameSubStr(const char * filePath);
    void writePadded(char ** dest, int64_t number, int width, char pad = ' ');
}

class Log
{
public:
    Log(int logLevel, const char* filename, int lineNo, bool halt = false);

    void operator()(const char * str) const {
        logImpl(std::string(str));
    }

    template<typename T1>
    void operator()(const char * str, const T1 & t1) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+1) == '1' && *(pStr+2) == '%') {
                LogDetails::output(msg, t1);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2>
    void operator()(const char * str, const T1 & t1, const T2 & t2) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3, typename T4>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else if (placeHolder == '4') { LogDetails::output(msg, t4); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else if (placeHolder == '4') { LogDetails::output(msg, t4); }
                else if (placeHolder == '5') { LogDetails::output(msg, t5); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5,
                                      const T6 & t6) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else if (placeHolder == '4') { LogDetails::output(msg, t4); }
                else if (placeHolder == '5') { LogDetails::output(msg, t5); }
                else if (placeHolder == '6') { LogDetails::output(msg, t6); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5,
                                      const T6 & t6, const T7 & t7) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else if (placeHolder == '4') { LogDetails::output(msg, t4); }
                else if (placeHolder == '5') { LogDetails::output(msg, t5); }
                else if (placeHolder == '6') { LogDetails::output(msg, t6); }
                else if (placeHolder == '7') { LogDetails::output(msg, t7); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5,
                                      const T6 & t6, const T7 & t7,  const T8 & t8) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else if (placeHolder == '4') { LogDetails::output(msg, t4); }
                else if (placeHolder == '5') { LogDetails::output(msg, t5); }
                else if (placeHolder == '6') { LogDetails::output(msg, t6); }
                else if (placeHolder == '7') { LogDetails::output(msg, t7); }
                else if (placeHolder == '8') { LogDetails::output(msg, t8); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    void operator()(const char * str, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5,
                                      const T6 & t6, const T7 & t7,  const T8 & t8, const T9 & t9) const {
        std::string msg; msg.reserve(128);
        const std::size_t strLen = strlen(str);
        for(std::size_t i = 0; i < strLen; ++i) {
            const char * pStr = str + i;
            if (*pStr == '%' && strLen >= i+3 && *(pStr+2) == '%') {
                const char placeHolder = *(pStr+1);
                if (placeHolder == '1') { LogDetails::output(msg, t1); }
                else if (placeHolder == '2') { LogDetails::output(msg, t2); }
                else if (placeHolder == '3') { LogDetails::output(msg, t3); }
                else if (placeHolder == '4') { LogDetails::output(msg, t4); }
                else if (placeHolder == '5') { LogDetails::output(msg, t5); }
                else if (placeHolder == '6') { LogDetails::output(msg, t6); }
                else if (placeHolder == '7') { LogDetails::output(msg, t7); }
                else if (placeHolder == '8') { LogDetails::output(msg, t8); }
                else if (placeHolder == '9') { LogDetails::output(msg, t9); }
                else fprintf(stderr, "Unexpected placeholder '%c'", placeHolder);
                i += 2;
            } else {
                msg.push_back(*pStr);
            }
        }
        logImpl(msg);
    }

private:
        void logImpl(const std::string & msg) const { log(logLevel_, filename_, lineNo_, halt_, msg); }
        static void log(int level, const char * file, int line, bool halt, const std::string & msgContent);

private:
    int logLevel_;
    const char* filename_;
    int lineNo_;
    bool halt_;
};
