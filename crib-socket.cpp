
#include "crib-socket.h"


Socket::Socket(int domain, int type, int protocol)
{
	fd = socket(domain, type, protocol);
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
