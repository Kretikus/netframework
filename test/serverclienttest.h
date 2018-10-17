#ifndef SERVERCLIENTTEST_H
#define SERVERCLIENTTEST_H

#include <server.h>
#include <workerthread.h>
#include <serialisation.h>
#include <consthelpers.hpp>
#include <retvalserialisation.h>
#include <remotecall.h>
#include <protocolbase.h>
#include <handlerbase.h>

#include <QObject>

#include <map>

#include <QDebug>

class ServerImpl
{
public:
    ServerImpl();

    void receivedData(const std::vector<char>&, ClientConnection*);
    bool isDataComplete(const std::vector<char>&);
    void clientStateChanged(ClientConnection*, ConnectionState::State);

private:
    Server * srv;
    std::map<unsigned int, ClientConnection*> connections;
};


class ServerClientTest : public QObject
{
    Q_OBJECT
public:
    ServerClientTest();
};

#endif // SERVERCLIENTTEST_H
