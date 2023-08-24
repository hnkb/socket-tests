
#pragma once

#include <system_error>

#if !defined(_WIN32)
#	include <sys/socket.h>
#	include <arpa/inet.h>
#else
#	include <winsock2.h>
#	include <ws2tcpip.h>
#endif


class Socket
{
public:
	Socket(int domain, int type, int protocol);
	Socket(const char* nodeName, const char* serviceName);
	~Socket();

	static std::system_error lastError();

#if !defined(_WIN32)
	Socket(int socketfd) : fd(socketfd) {}
	operator int() { return fd; }

private:
	int fd = -1;
#else
	Socket(SOCKET socketfd) : fd(socketfd) {}
	operator SOCKET() { return fd; }

private:
	SOCKET fd = INVALID_SOCKET;
#endif
};
