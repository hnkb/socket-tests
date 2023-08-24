
#include "crib-socket.h"
#include "chat.h"
#include <iostream>


void client(const char* server_ip_address)
{
	//Socket s("localhost", "HTTPS");
	//Socket p("127.0.0.1", "HTTPS");
	//Socket q("localhost", "80");
	//Socket q1("localhost", "16432");
	//Socket q2("52.59.194.5", "16432");
	//Socket q3("dev.hnkb.de", "HTTP");

	//return;


	Socket sock(AF_INET, SOCK_STREAM, 0);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(16432);
	inet_pton(AF_INET, server_ip_address, &address.sin_addr);

	if (connect(sock, (sockaddr*)&address, sizeof(address)) < 0)
		throw Socket::lastError();

	std::cout << "Connected to server " << server_ip_address << ". You can now chat...\n";

	chat(sock);
}

int main()
{
	try
	{
		// client("127.0.0.1");
		client("52.59.194.5");
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ERROR: " << ex.what() << "\n";
	}

	return 0;
}
