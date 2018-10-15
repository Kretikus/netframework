#pragma once

#include <functional>
#include <mutex>
#include <vector>

class ClientConnection;

namespace ConnectionState {
    enum State {
        Disconnected = 0,
        Connected = 1
    };
}

class Server
{
public:
	typedef std::function<void(const std::vector<char>&, ClientConnection*)> DataCallback;
	typedef std::function<bool(const std::vector<char>&)> DataCompleteCallback;
    typedef std::function<void(ClientConnection*, ConnectionState::State)> ClientStateChangedCallback;
public:
	Server(int port, const DataCallback & dataCallback, const DataCompleteCallback & dataCompleteCallback,
           const ClientStateChangedCallback & clientDisconnectCallback);
	~Server();

	bool init();
    bool listen();
	bool poll();

private:
	friend class ClientConnection;
	class Impl;
	std::unique_ptr<Impl> _d;
};


class ClientConnection
{
public:

public:
    ClientConnection(Server::Impl * srvImpl, unsigned int fd) : _srvImpl(srvImpl), _fd(fd) {}

    void sendData(const std::vector<char> & data);

    bool receiveDataFromSocket();
    void sendDataToSocket();

    Server::Impl * _srvImpl;
    unsigned int _fd;
    std::mutex _mutex;
    std::vector<char> receivedData;
    std::vector<char> dataToSend;
};
