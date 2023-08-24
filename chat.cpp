
#include "chat.h"
#include <thread>
#include <cstring>
#include <iostream>

using namespace std::chrono_literals;


std::atomic<bool> alive = true;

void connectionFunc(Socket& sock)
{
	char buffer[1024];
	while (alive)
	{
		auto received = recv(sock, buffer, sizeof(buffer), 0);
		buffer[received] = 0;
		std::cout << " [other]: " << buffer << "\n";
	}
}

void inputFunc(Socket& sock)
{
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), stdin))
	{
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

	alive = false;
}


void chat(Socket& sock)
{
	std::thread connectionThread(connectionFunc, std::ref(sock));
	std::thread inputThread(inputFunc, std::ref(sock));

	connectionThread.join();
	inputThread.join();
}
