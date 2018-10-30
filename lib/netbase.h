#ifndef NETBASE_H
#define NETBASE_H

#include <string>

typedef std::string ByteBuffer;

class NetBase
{
public:
    NetBase(unsigned short port, const std::string & certificateDir);
    ~NetBase();

    void start();
    void stop();

private:
    class Impl;
    Impl* impl_;
};

#endif // NETBASE_H
