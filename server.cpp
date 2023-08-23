
#include "crib-socket.h"
#include <arpa/inet.h>

#include <cstring>
#include <iostream>


void server()
{
	Socket sock(AF_INET, SOCK_STREAM, 0);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(16432);

	{
		int reuse = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
			throw Socket::lastError();
	}

	if (bind(sock, (sockaddr*)&address, sizeof(address)) < 0)
		throw Socket::lastError();

	std::cout << "Listening on port " << ntohs(address.sin_port) << "...\n";

	if (listen(sock, SOMAXCONN) < 0)
		throw Socket::lastError();

	sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
	auto clientfd = accept(sock, (sockaddr*)&client_address, &client_address_len);

	if (clientfd < 0)
		throw Socket::lastError();

	Socket client(clientfd);

	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
	std::cout << "Connected to client " << client_ip << "...\n";

	char buffer[1024];

	auto rec = recv(client, buffer, sizeof(buffer), 0);
	buffer[rec] = 0;
	std::cout << " [client]: " << buffer << "\n";

	fgets(buffer, sizeof(buffer), stdin);
	auto sent = send(client, buffer, strlen(buffer), 0);
}

int main()
{
	try
	{
		server();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ERROR: " << ex.what() << "\n";
	}

	return 0;
}
