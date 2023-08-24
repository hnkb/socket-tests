
#include "crib-socket.h"
#include <cstring>
#include <iostream>


void client(const char* server_ip_address)
{
	Socket sock(AF_INET, SOCK_STREAM, 0);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(16432);
	inet_pton(AF_INET, server_ip_address, &address.sin_addr);

	if (connect(sock, (sockaddr*)&address, sizeof(address)) < 0)
		throw Socket::lastError();

	std::cout << "Connected to server " << server_ip_address << "...\n";


	char buffer[1024];

	fgets(buffer, sizeof(buffer), stdin);
	auto sent = send(sock, buffer, strlen(buffer), 0);

	auto received = recv(sock, buffer, sizeof(buffer), 0);
	buffer[received] = 0;
	std::cout << " [server]: " << buffer << "\n";
}

int main()
{
	try
	{
		client("127.0.0.1");
		// client("52.59.194.5");
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ERROR: " << ex.what() << "\n";
	}

	return 0;
}
