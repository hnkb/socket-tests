
#include "crib-socket.h"
#include <iostream>

Socket::Socket(int domain, int type, int protocol)
{
	fd = socket(domain, type, protocol);
	if (fd == -1)
		throw lastError();
}

Socket::Socket(const char* nodeName, const char* serviceName)
{
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;
	// hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result = nullptr;
	auto ret = getaddrinfo(nodeName, serviceName, &hints, &result);
	if (ret != 0)
	{
		printf("getaddrinfo failed with error: %d\n", ret);
		return;
	}

	for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		std::cout << "Family: " << ptr->ai_family << ", Protocol: " << ptr->ai_protocol
				  << ", Address length: " << ptr->ai_addrlen << "\n";

		char buf[INET6_ADDRSTRLEN], ser[500];
		//inet_ntop(ptr->ai_addr->sa_family, ptr->ai_addr->sa_data, buf, sizeof(buf));
		getnameinfo(
			ptr->ai_addr,
			ptr->ai_addrlen,
			buf,
			sizeof(buf),
			ser,
			sizeof(ser),
			NI_NUMERICHOST | NI_NUMERICSERV);
		std::cout << "   " << buf << "         " << ser << "\n";
		if (ptr->ai_canonname)
		std::cout << "   " << ptr->ai_canonname << "\n";
		// iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	}

	freeaddrinfo(result);
}


#if !defined(_WIN32)

#	include <unistd.h>

Socket::~Socket()
{
	close(fd);
}

std::system_error Socket::lastError()
{
	return std::system_error(std::error_code(errno, std::generic_category()));
}

#else

Socket::~Socket()
{
	closesocket(fd);
}

std::system_error Socket::lastError()
{
	auto code = WSAGetLastError();

	char* msg;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&msg,
		0,
		nullptr);

	// if (msg)
	//	LocalFree(msg);

	// TODO: this is problematic, as we never get a chance to free msg by calling LocalFree
	return std::system_error(std::error_code(code, std::generic_category()), msg);
}


#	include <stdio.h>

namespace
{
	class WinSockStartup
	{
	public:
		WinSockStartup()
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);

			// TODO: find a way to check for possible error return value
			// It's not okay to throw here, because this will run on app startup
		}

		~WinSockStartup() { WSACleanup(); }
	};

	WinSockStartup globalWinSockStartup;
}

#	pragma comment(lib, "Ws2_32.lib")
#	pragma comment(lib, "Mswsock.lib")
#	pragma comment(lib, "AdvApi32.lib")

#endif
