
#include "chat.h"
#include <cstring>
#include <iostream>


#if !defined(_WIN32)

#	include <unistd.h>
#	include <poll.h>

class combinedPoll
{
public:
	combinedPoll(Socket& sock)
	{
		sources[0].fd = STDIN_FILENO;
		sources[0].events = POLLIN;
		sources[1].fd = sock;
		sources[1].events = POLLIN;
	}

	void poll()
	{
		::poll(sources, sizeof(sources) / sizeof(pollfd), -1);
		// TODO: check poll() return value for error
	}

	bool socketClosed() const { return sources[1].revents & (POLLERR | POLLHUP); }
	bool socketHasData() const { return sources[1].revents & POLLIN; }
	bool stdinHasData() const { return sources[0].revents & POLLIN; }

private:
	pollfd sources[2];
};

#else

#endif


void chat(Socket& sock)
{
	combinedPoll sources(sock);

	char buffer[1024];

	while (true)
	{
		sources.poll();

		if (sources.socketClosed())
		{
			std::cout << "Connection closed by the other party.\n\n";
			break;
		}

		if (sources.socketHasData())
		{
			auto received = recv(sock, buffer, sizeof(buffer), 0);
			buffer[received] = 0;
			if (received == 0)
			{
				std::cout << "Connection closed by the other party.\n\n";
				break;
			}
			std::cout << " [other]: " << buffer << "\n";
		}

		if (sources.stdinHasData())
		{
			fgets(buffer, sizeof(buffer), stdin);
			auto len = strlen(buffer);
			if (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')
			{
				buffer[len - 1] = 0;
				len--;
			}

			if (len == 0 || std::string("quit") == buffer || std::string("exit") == buffer)
				break;

			auto sent = send(sock, buffer, len, 0);
			if (sent != len)
				throw Socket::lastError();
		}
	}
}
