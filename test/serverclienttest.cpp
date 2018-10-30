#include "testutil.h"
#include "serverclienttest.h"

#include <coreproto.h>

#include <clientnet.h>
#include <netbase.h>

#include <thread>

//ServerImpl::ServerImpl()
//    : srv(nullptr)
//{
//    srv = new Server(9000,
//                     [&](const std::vector<char>& d, ClientConnection* conn) {receivedData(d, conn);},
//                     [&](const std::vector<char>& d) -> bool { return isDataComplete(d); },
//                     [&](ClientConnection* conn, ConnectionState::State state) { clientStateChanged(conn, state); }
//            );
//}

//void ServerImpl::receivedData(const std::vector<char> & /*d*/, ClientConnection * conn)
//{
//    if (!conn) return;
//}

//bool ServerImpl::isDataComplete(const std::vector<char> & d)
//{
//    return coreproto::isPacketComplete(d);
//}

//void ServerImpl::clientStateChanged(ClientConnection * conn, ConnectionState::State state)
//{
//    if (!conn) return;
//    if (state == ConnectionState::Connected) {
//        connections[conn->_fd] = conn;
//    } else {
//        connections.erase(conn->_fd);
//    }
//}

REGISTER_TEST(ServerClientTest);

ServerClientTest::ServerClientTest()
{
}

void ServerClientTest::testClientNet()
{

    qDebug() << "Starting netbase";
    NetBase netbase(7777, "/Users/roman/Development/experiments");
    std::thread thread(std::bind(&NetBase::start, &netbase));
    qDebug() << "Listening to 7777";

    ClientNet client("/Users/roman/Development/experiments");
    qDebug() << "Connecting to 7777";
    client.connect("localhost", 7777);
    qDebug() << "Connected";
    std::string ret = client.send("12345678");
    qDebug() << "RET: " << ret.c_str();

    std::string ret2 = client.send("897654321");
    qDebug() << "RET: " << ret2.c_str();

    netbase.stop();
    thread.join();
}


