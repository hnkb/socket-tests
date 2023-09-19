
#include "chat.h"
#include <iostream>


void chat(Socket& sock)
{
	HANDLE sources[2];

	const int ix_socket = 0;
	const int ix_stdin = 1;

	sources[ix_socket] = WSACreateEvent();
	if (sources[ix_socket] == WSA_INVALID_EVENT)
		throw Socket::lastError();

	if (WSAEventSelect(sock, sources[ix_socket], FD_READ | FD_CLOSE))
		throw Socket::lastError();

	//auto result = WaitForMultipleObjectsEx(1, sources, FALSE, INFINITE, FALSE);
	//std::cout << "Event triggered " << result << "\n";

	sources[ix_stdin] = GetStdHandle(STD_INPUT_HANDLE);

	char buffer[1024];


	//recv(sock, buffer, sizeof(buffer), 0);


	//const HANDLE sources[] { GetStdHandle(STD_INPUT_HANDLE), WSACreateEvent(), INVALID_HANDLE_VALUE };
	//WSAEventSelect(sock, sources[1], FD_READ);

	//INPUT_RECORD r[512];
	//DWORD read;
	//ReadConsoleInput(sources[ix_stdin], r, 512, &read);
	//printf("Read: %d\n", read);

	for (int i = 0; i < 13; i++)
	{
		auto result = WaitForMultipleObjects(1, sources, FALSE, INFINITE);

		std::cout << "Return value " << result << "\n";

		if (result == WAIT_OBJECT_0 + ix_socket)
		{
			auto received = recv(sock, buffer, sizeof(buffer), 0);
			buffer[received] = 0;
			std::cout << " [other]: " << buffer << "\n";
			// WSAResetEvent(sources[result]);
		}

		if (result == WAIT_OBJECT_0 + ix_stdin)
		{
			//fgets(buffer, sizeof(buffer), stdin);
			INPUT_RECORD r[512];
			DWORD read;
			ReadConsoleInput(sources[ix_stdin], r, 512, &read);
			printf("Read: %d\n", read);
		}


		WSAResetEvent(sources[ix_socket]);
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
