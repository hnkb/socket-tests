
#include "crib-socket.h"
#include <iostream>
// #include <sys/types.h>
// #include <sys/socket.h>
#	include <unistd.h>

Socket::Socket(int domain, int type, int protocol)
{
	fd = socket(domain, type, protocol);
	if (fd == -1)
		throw lastError();
}

Socket::Socket(const char* nodeName, const char* serviceName, bool isPassive, int type)
{
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;
	// hints.ai_flags = AI_CANONNAME;
	// hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = isPassive ? 0 : AI_PASSIVE;

	addrinfo* result = nullptr;
	auto ret = getaddrinfo(nodeName, serviceName, &hints, &result);
	if (ret != 0)
	{
		printf(
			"getaddrinfo failed with error: %d\n%s\n\n",
			ret,
			std::system_error(std::error_code(ret, std::generic_category())).what());
		return;
	}

	for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		std::cout << "Family: " << ptr->ai_family << ", Protocol: " << ptr->ai_protocol
				  << ", Address length: " << ptr->ai_addrlen << "\n";

		char buf[INET6_ADDRSTRLEN], ser[500];
		// inet_ntop(ptr->ai_addr->sa_family, ptr->ai_addr->sa_data, buf, sizeof(buf));
		getnameinfo(
			ptr->ai_addr,
			ptr->ai_addrlen,
			buf,
			sizeof(buf),
			ser,
			sizeof(ser),
			0 | NI_NUMERICHOST | NI_NUMERICSERV);
		std::cout << "   " << buf << "         " << ser << "\n";
		if (ptr->ai_canonname)
			std::cout << "   " << ptr->ai_canonname << "\n";

		fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (fd == -1)
			throw lastError();

		int iResult;
		if (isPassive)
			iResult = bind(fd, ptr->ai_addr, ptr->ai_addrlen);
		else
			iResult = connect(fd, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult < 0)
		{
			// closesocket(fd);
			close(fd);
			fd = -1;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (fd == -1)
		throw lastError();
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
