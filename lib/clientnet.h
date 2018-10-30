#ifndef CLIENTNET_H
#define CLIENTNET_H

#include <string>

class ClientNet
{
public:
    ClientNet(const std::string & sslVerifyFile);
    ~ClientNet();

    void connect(const std::string & addr, int port);

    std::string send(const std::string & data);


private:
    class Impl;
    Impl * impl_;
};

#endif // CLIENTNET_H
