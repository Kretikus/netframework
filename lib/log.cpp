#include "log.h"

#include <cstdio>
#include <sys/time.h>
#include <string>
#include <thread>

#ifdef __APPLE__
namespace {
    uint64_t getThreadId()
    {
        uint64_t tid;
        pthread_threadid_np(nullptr, &tid);
        return tid;
    }
}
#endif

void LogDetails::output(std::string & msg, double val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, int16_t val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, int32_t val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, int64_t val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, uint16_t val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, uint32_t val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, uint64_t val) {
    msg.append( std::to_string(val) );
}

void LogDetails::output(std::string & msg, const std::string & val) {
    msg.append( val );
}

const char * LogDetails::filenameSubStr(const char * filePath)
{
    // strategy: Find last / or
    const size_t size = ::strlen(filePath);
    const char * pos = filePath + size;
    for(;;--pos) {
        if (pos == filePath) return pos;
        if (*pos == '/' || *pos == '\\') return pos+1;
    }
    return pos;
}

inline void LogDetails::writePadded(char ** dest, int64_t number, int width, char pad)
{
    *dest += width;
    char * pos = *dest;
    for (int i = 0 ; i < width ; ++i) {
        if (number == 0 && i > 0) {
                *(--pos) = pad;
        } else {
                *(--pos) = '0' + number % 10;
                number /= 10;
        }
    }
}

class LogImpl
{
public:
    LogImpl() : logFile_(nullptr)
    {
        logFile_ = std::fopen("debug.log", "at");
    }

    ~LogImpl() {
        if (logFile_) {
            std::fclose(logFile_);
        }
    }

    static LogImpl* instance() {
        static LogImpl* impl = new LogImpl;
        return impl;
    }

    std::FILE * logFile_;
};

Log::Log(int logLevel, const char * filename, int lineNo, bool halt)
    : logLevel_(logLevel), filename_(filename), lineNo_(lineNo), halt_(halt)
{
}

void Log::log(int level, const char * file, int line, bool halt, const std::string & msgContent)
{
    // skip processing if output is not valid
    if (!LogImpl::instance()->logFile_) return;

    std::string msg;
    msg.reserve(256);
    msg.resize(60);
    char * pos = static_cast<char *>(&msg[0]);

    // timestamp
    struct timeval tp;
    gettimeofday(&tp, nullptr);

    struct tm* time = localtime(&tp.tv_sec);

    LogDetails::writePadded(&pos, 1900 + time->tm_year, 4);
    *(pos++) = '-';
    LogDetails::writePadded(&pos, time->tm_mon + 1, 2, '0');
    *(pos++) = '-';
    LogDetails::writePadded(&pos, time->tm_mday, 2, '0');
    *(pos++) = ' ';
    LogDetails::writePadded(&pos, time->tm_hour, 2, '0');
    *(pos++) = ':';
    LogDetails::writePadded(&pos, time->tm_min, 2, '0');
    *(pos++) = ':';
    LogDetails::writePadded(&pos, time->tm_sec, 2, '0');
    *(pos++) = '.';
    LogDetails::writePadded(&pos, tp.tv_usec / 1000, 3, '0');
    *(pos++) = ' ';
    LogDetails::writePadded(&pos, level, 1);
    *(pos++) = ' ';
    *(pos++) = 't';
    *(pos++) = ':';
    *(pos++) = ' ';
    LogDetails::writePadded(&pos, (int)getThreadId(), 4);
    *(pos++) = ' ';

    const char * filename = ::LogDetails::filenameSubStr(file);
    for (int i = 0 ; i < 20 ; ++i) *(pos++) = *filename ? *(filename++) : ' ';
    LogDetails::writePadded(&pos, line, 5);
    *(pos++) = ' ';

    msg.append(msgContent);
    std::fputs(msg.c_str(), LogImpl::instance()->logFile_);
    std::fputs("\n", LogImpl::instance()->logFile_);
    fflush(LogImpl::instance()->logFile_);

    if(halt) {
        assert(false);
    }
}

std::string LogDetails::getThreadString()
{
    return std::to_string(getThreadId());
}
