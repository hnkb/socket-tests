
#include "crib-socket.h"
#include <sys/socket.h>
#include <unistd.h>


Socket::Socket(int domain, int type, int protocol)
{
	fd = socket(domain, type, protocol);
	if (fd < 0)
		throw lastError();
}

Socket::~Socket()
{
	close(fd);
}

std::system_error Socket::lastError()
{
	return std::system_error(std::error_code(errno, std::generic_category()));
}
