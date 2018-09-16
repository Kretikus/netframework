#include "server.h"

#include "netutil.h"

#include <exception>
#include <map>
#include <set>
#include <string>
#include <mutex>

#define DEFAULT_WAIT    500


class Server::Impl
{
public:
    Impl(int port, const DataCallback & dataCallback, const DataCompleteCallback & dataCompleteCallback, const ClientDisconnectCallback & clientDisconnectCallback)
        : _port(port), _dataCallback(dataCallback), _dataCompleteCallback(dataCompleteCallback), _clientDisconnectCallback(clientDisconnectCallback)
    {
    }
    ~Impl() {
        if (listenSocket) closesocket(listenSocket);
    }

    int _port;
    DataCallback  _dataCallback;
    DataCompleteCallback _dataCompleteCallback;
    ClientDisconnectCallback _clientDisconnectCallback;

    WSAUser wsaUser;
    SOCKET listenSocket = NULLSOCKET;

    std::mutex mutex;
    std::map<SOCKET, ClientConnection*> clientSockets;
    std::set<ClientConnection*> writingSockets;
};

Server::Server(int port, const DataCallback & dataCallback, const DataCompleteCallback & dataCompleteCallback, const ClientDisconnectCallback & clientDisconnectCallback)
    : _d(new Impl(port, dataCallback, dataCompleteCallback, clientDisconnectCallback))
{}

Server::~Server()
{
}

bool Server::init()
{
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    AddrInfoContainer resultContainer;
    const std::string portString = std::to_string(_d->_port);
    int iResult = getaddrinfo(NULL, portString.c_str(), &hints, &resultContainer);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return false;
    }

    // Create a SOCKET for connecting to server
    struct addrinfo *result = resultContainer.result;
    _d->listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_d->listenSocket == NULLSOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        return false;
    }

    // Setup the TCP listening socket
    iResult = bind(_d->listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %ld\n", WSAGetLastError());
        return 1;
    }

    return true;
}

bool Server::listen()
{
    int iResult = ::listen(_d->listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %ld\n", WSAGetLastError());
        return false;
    }

    return true;
}

bool Server::poll()
{
#ifdef _WIN32
    ULONG               uNonBlockingMode = 1;
    CHAR                buf[MAX_PATH] = { 0 };
    HANDLE              hThread = NULL;
    DWORD               dwThreadId = 0;

    std::vector<WSAPOLLFD> pollfds;
    WSAPOLLFD fd;
    fd.fd = _d->listenSocket;
    fd.events = POLLRDNORM;
    pollfds.push_back(fd);

    while (true) {
        // check for reading
        for (unsigned int i = 0; i < pollfds.size(); ++i) pollfds[i].events = POLLRDNORM;

        int ret = 0;
        if (SOCKET_ERROR == (ret = WSAPoll(&pollfds[0], pollfds.size(), DEFAULT_WAIT)))
        {
            printf("WSA Poll failed");
            return false;
        }

        // check for new incoming connections
        if (pollfds[0].revents & POLLRDNORM) {
            printf("Incoming connection!");
            SOCKET asock;
            if (INVALID_SOCKET == (asock = accept(pollfds[0].fd, NULL, NULL)))
            {
                printf("Accept failed!");
                return false;
            }
            _d->clientSockets[asock] = new ClientConnection(_d.get(), asock);
            WSAPOLLFD fd;
            fd.fd = asock;
            fd.events = POLLRDNORM;
            pollfds.push_back(fd);
        }

        for (unsigned int i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLRDNORM) {
                ClientConnection * conn = _d->clientSockets[pollfds[i].fd];
                if (conn) {
                    conn->receiveDataFromSocket();
                }
            }
            // detect disconnect
            else if (pollfds[i].revents & POLLHUP) {
                ClientConnection * conn = _d->clientSockets[pollfds[i].fd];
                _d->writingSockets.erase(conn);
                _d->clientSockets.erase(pollfds[i].fd);
                _d->_clientDisconnectCallback(conn);
                delete conn;
            }

        }

        /// handle sending data

        std::set<ClientConnection*> connectionsToWrite;
        {
            std::scoped_lock lock(_d->mutex);
            std::swap(_d->writingSockets, connectionsToWrite);
        }
        std::vector<WSAPOLLFD> wPollfds;
        for (auto conn : connectionsToWrite) {
            WSAPOLLFD fd;
            fd.fd = conn->_fd;
            fd.events = POLLWRNORM;
            wPollfds.push_back(fd);
        }
        if (wPollfds.size() > 0) {
            if (SOCKET_ERROR == (ret = WSAPoll(&wPollfds[0], wPollfds.size(), DEFAULT_WAIT)))
            {
                printf("WSA Poll (write) failed");
                return false;
            }

            for (unsigned int i = 0; i < wPollfds.size(); ++i) {
                if (wPollfds[i].revents & POLLWRNORM) {
                    ClientConnection * conn = _d->clientSockets[wPollfds[i].fd];
                    if (conn) {
                        conn->sendDataToSocket();
                    }
                }
                else if (wPollfds[i].revents != 0) {
                    printf("REVENT: %d", wPollfds[i].revents);
                }
            }
        }
    }
#else

    int listenSocket = _d->listenSocket;
    fd_set active_fd_set, read_fd_set;
    fd_set write_fd_set;
    /* Initialize the set of active sockets. */
    FD_ZERO (&active_fd_set);
    FD_SET (listenSocket, &active_fd_set);

    while (1)
    {
        std::set<ClientConnection*> connectionsToWrite;
        {
            std::lock_guard<std::mutex> lock(_d->mutex);
            std::swap(_d->writingSockets, connectionsToWrite);
        }
        FD_ZERO (&write_fd_set);
        for (auto conn: connectionsToWrite) {
            FD_SET(conn->_fd, &write_fd_set);
        }

        /* Block until input arrives on one or more active sockets. */
        read_fd_set = active_fd_set;
        if (select (FD_SETSIZE, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
        {
            printf("Error in select\n");
            return false;
        }

        /* Service all the sockets with input pending. */
        for (int i = 0; i < FD_SETSIZE; ++i)
        {
            bool deleteSocket = false;
            if (FD_ISSET (i, &read_fd_set))
            {
                if (i == listenSocket)
                {
                    /* Connection request on original socket. */
                    int newConn;
                    struct sockaddr_in clientname;
                    socklen_t size = sizeof (clientname);
                    newConn = ::accept(listenSocket,
                                       (struct sockaddr *) &clientname,
                                       &size);
                    if (newConn < 0)
                    {
                        printf("Error accepting new connection\n");
                        return false;
                    }
                    fprintf (stderr,
                             "Server: connect from host %s, port %hd.\n",
                             inet_ntoa (clientname.sin_addr),
                             ntohs (clientname.sin_port));
                    FD_SET (newConn, &active_fd_set);
                    _d->clientSockets[newConn] = new ClientConnection(_d.get(), newConn);
                }
                else
                {
                    ClientConnection * conn = _d->clientSockets[i];
                    if (conn) {
                        if (!conn->receiveDataFromSocket()) {
                            ClientConnection * conn = _d->clientSockets[i];
                            _d->writingSockets.erase(conn);
                            _d->clientSockets.erase(i);
                            _d->_clientDisconnectCallback(conn);
                            delete conn;
                            FD_CLR (i, &active_fd_set);
                            deleteSocket = true;
                        }
                    }
                }
            }

            if (FD_ISSET (i, &write_fd_set)) {
                ClientConnection * conn = _d->clientSockets[i];
                if (conn) {
                    conn->sendDataToSocket();
                }
            }

            if (deleteSocket) {
                ::close (i);
            }
        }
    }

#endif

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ClientConnection::sendData(const std::vector<char> & data)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        dataToSend.insert(dataToSend.end(), data.begin(), data.end());
    }
    std::lock_guard<std::mutex> lock(_srvImpl->mutex);
    _srvImpl->writingSockets.insert(this);
}

bool ClientConnection::receiveDataFromSocket() {
    std::vector<char> buf(MAX_PATH, 0);
    int ret = recv(_fd, &buf[0], buf.size(), 0);
    if (ret == 0) return false;
    buf.resize(ret);
    if (receivedData.empty() && _srvImpl->_dataCompleteCallback(buf)) {
        _srvImpl->_dataCallback(buf, this);
    }
    else {
        receivedData.insert(receivedData.end(), buf.begin(), buf.end());
        if (_srvImpl->_dataCompleteCallback(receivedData)) {
            _srvImpl->_dataCallback(receivedData, this);
            receivedData.clear();
        }
    }
    return true;
}

void ClientConnection::sendDataToSocket()
{
    std::vector<char> data;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::swap(dataToSend, data);
    }
    unsigned int ret = 0;
    if (SOCKET_ERROR == (ret = send(_fd, &data[0], data.size(), 0)))
    {
        printf("could not send data to socket.");
    }
    if (ret < data.size()) {
        printf("Could only send %d bytes from %lu to socket.", ret, data.size());
    }
}
