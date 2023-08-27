
#include "crib-socket.h"
#include "chat.h"
#include <iostream>


void server()
{
	Socket sock("::", "16432", true);
	// Socket sock(AF_INET, SOCK_STREAM, 0);

	// sockaddr_in address;
	// address.sin_family = AF_INET;
	// address.sin_addr.s_addr = INADDR_ANY;
	// address.sin_port = htons(16432);

	{
		int reuse = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
			throw Socket::lastError();
	}

	// if (bind(sock, (sockaddr*)&address, sizeof(address)) < 0)
	// 	throw Socket::lastError();

	// std::cout << "Listening on port " << ntohs(address.sin_port) << "...\n";

	if (listen(sock, SOMAXCONN) < 0)
		throw Socket::lastError();

	sockaddr client_address;
	// sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
	auto clientfd = accept(sock, &client_address, &client_address_len);

	if (clientfd < 0)
		throw Socket::lastError();

	Socket client(clientfd);

	char client_ip[INET_ADDRSTRLEN];
	getnameinfo(
		&client_address,
		client_address_len,
		client_ip,
		sizeof(client_ip),
		nullptr,
		0,
		0);
	// inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
	std::cout << "Connected to client " << client_ip << ". You can now chat...\n";

	chat(client);
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
