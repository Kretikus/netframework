#pragma once

#ifdef _WIN32
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#include <string>

#ifdef _WIN32

#define NULLSOCKET NULL

class WSAUser {
	WSADATA data;
public:
	WSAUser() {
		int iResult = WSAStartup(MAKEWORD(2, 2), &data);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			throw std::runtime_error("Could not call WSASTartup");
		}
	}
	~WSAUser() { WSACleanup(); }
};

#else
struct WSAUser {};

typedef int SOCKET;

enum Constants {
    MAX_PATH = 1024*64,
    NULLSOCKET = 0,
    SOCKET_ERROR = 1
};

inline void closesocket(int fd) {
    ::close(fd);
}

inline void ZeroMemory(void * p,  int len) {
    memset(p, 0, len);
}

inline long WSAGetLastError() { return errno; }

#endif

class AddrInfoContainer {
public:
    ~AddrInfoContainer() { freeaddrinfo(result); }

    addrinfo ** operator&() { return &result; }

    struct addrinfo *result = NULL;
};

inline std::string addrString(struct addrinfo * ptr)
{
    struct in_addr  *addr;
    if (ptr->ai_family == AF_INET) {
        struct sockaddr_in *ipv = (struct sockaddr_in *)ptr->ai_addr;
        addr = &(ipv->sin_addr);
    }
    else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
        addr = (struct in_addr *) &(ipv6->sin6_addr);
    }
    char ipAddress[INET6_ADDRSTRLEN];
    inet_ntop(ptr->ai_family, addr, ipAddress, sizeof(ipAddress));
    return ipAddress;
}
