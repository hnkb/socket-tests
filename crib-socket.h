
#pragma once

#include <system_error>


class Socket
{
public:
	Socket(int domain, int type, int protocol);
	Socket(int socketfd) : fd(socketfd) {}
	~Socket();
	operator int() { return fd; }

	static std::system_error lastError();

private:
	int fd = -1;
};
