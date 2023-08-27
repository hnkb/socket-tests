
#include "chat.h"
#include <iostream>


void chat(Socket& sock)
{
	char buffer[1024];


	const HANDLE sources[] { GetStdHandle(STD_INPUT_HANDLE), WSACreateEvent() };
	WSAEventSelect(sock, sources[1], FD_READ | FD_OOB | FD_CLOSE);

	for (int i = 0; i < 3; i++)
	{
		auto result = WaitForMultipleObjectsEx(2, sources, FALSE, INFINITE, FALSE);

		std::cout << "Return value " << result << "\n";
		fgets(buffer, sizeof(buffer), stdin);
	}

#if 0
	pollfd sources[] {
		{ STDIN_FILENO, POLLIN, 0 },
		{ (int)sock,    POLLIN, 0 }
	};

	char buffer[1024];

	while (true)
	{
		poll(sources, sizeof(sources) / sizeof(pollfd), -1);
		// TODO: check poll() return value for error

		if (sources[1].revents & (POLLERR | POLLHUP))
		{
			std::cout << "Connection closed by the other party.\n\n";
			break;
		}

		if (sources[1].revents & POLLIN)
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

		if (sources[0].revents & POLLIN)
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
#endif
}
