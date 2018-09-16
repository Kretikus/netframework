#include "client.h"

#include "netutil.h"

class Client::Impl
{
public:
    ~Impl() {
        if (connectSocket) {
            closesocket(connectSocket);
        }
    }

    WSAUser wsa;
    SOCKET connectSocket = NULLSOCKET;
};

Client::Client()
    : _d(new Impl)
{}


Client::~Client()
{}

bool Client::connect(const std::string & srvname, unsigned int port)
{
    AddrInfoContainer addrInfocontainer;
    {
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        const std::string portAsString = std::to_string(port);

        int iResult = getaddrinfo(srvname.c_str(), portAsString.c_str(), &hints, &addrInfocontainer);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            return false;
        }
    }
    struct addrinfo *result = addrInfocontainer.result;

    // Attempt to connect to an address until one succeeds
    for (struct addrinfo * ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        _d->connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (_d->connectSocket == NULLSOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            return false;
        }

        // Connect to server.
        int iResult = ::connect(_d->connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        std::string ipStr = addrString(ptr);
        if (iResult == NULLSOCKET) {
            printf("Could not connect to: %s \n", ipStr.c_str());
            continue;
        }
        else {
            printf("Connected to: %s\n", ipStr.c_str());
        }
        break;
    }
    return true;
}
