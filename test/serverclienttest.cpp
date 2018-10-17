#include "serverclienttest.h"

#include <coreproto.h>

ServerImpl::ServerImpl()
    : srv(nullptr)
{
    srv = new Server(9000,
                     [&](const std::vector<char>& d, ClientConnection* conn) {receivedData(d, conn);},
                     [&](const std::vector<char>& d) -> bool { return isDataComplete(d); },
                     [&](ClientConnection* conn, ConnectionState::State state) { clientStateChanged(conn, state); }
            );
}

void ServerImpl::receivedData(const std::vector<char> & /*d*/, ClientConnection * conn)
{
    if (!conn) return;
}

bool ServerImpl::isDataComplete(const std::vector<char> & d)
{
    return coreproto::isPacketComplete(d);
}

void ServerImpl::clientStateChanged(ClientConnection * conn, ConnectionState::State state)
{
    if (!conn) return;
    if (state == ConnectionState::Connected) {
        connections[conn->_fd] = conn;
    } else {
        connections.erase(conn->_fd);
    }
}

ServerClientTest::ServerClientTest()
{
}
