// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#undef UNICODE

#define WIN32_LEAN_AND_MEAN
//
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "..\shared\network_wrapper.h"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "1337"
#define DEFAULT_BUFLEN 1024


int main()
{
	Wsa_data wsaData{};
	int iResult = 0;


	if (wsaData.result != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
		return 1;
	}

	std::string server_address_q;
	std::cout << "Enter the server hostname or IPv4 address" << std::endl;
	std::cin >> server_address_q;
	std::cin.ignore();
	PCSTR server_address = server_address_q.c_str();

	Addr_info name_hint{};
	name_hint.hints.ai_family = AF_INET;
	name_hint.hints.ai_socktype = SOCK_STREAM;
	name_hint.hints.ai_protocol = IPPROTO_TCP;
	//////setting the AI_PASSIVE flag here means we'll bind a socket
	////name_hint.hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &name_hint.hints, &name_hint.result);
	if (iResult != 0)
	{

		std::cout << "getaddrinfo failed" << std::endl;
		return 1;
	}

	Net_socket ConnectSocket(name_hint);
	if (ConnectSocket.socket_instance == INVALID_SOCKET)
	{
		std::cout << "ListenSocket failed" << std::endl;
		return 1;
	}

	iResult = connect(ConnectSocket.socket_instance, name_hint.result->ai_addr, (int)name_hint.result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed" << std::endl;
		return 1;
	}

	if (ConnectSocket.socket_instance == INVALID_SOCKET)
	{
		std::cout << "Unable to connect to server" << std::endl;
		return 1;
	}

	//setup buffers for send and receive
	int recvbuflen = DEFAULT_BUFLEN;
	//char sendbuff[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN]{};
	bool running = true;

	while (running)
	{
		std::string sendbuff_string;
		u_int64 bytes_packaged = 0;
		//send data
		std::cout << "Enter the message and hit enter" << std::endl;
		
		std::getline(std::cin, sendbuff_string);
		char sendbuff[DEFAULT_BUFLEN]{};
		std::cout << "sendbuff_string.size() is " << sendbuff_string.size() << std::endl;

		while (bytes_packaged < sendbuff_string.size())
		{
			for (uint64_t i = 0; i < DEFAULT_BUFLEN - 1; ++i)
			{
				if (bytes_packaged < sendbuff_string.size())
				{
					sendbuff[i] = sendbuff_string[bytes_packaged];
					++bytes_packaged;
				}
				else
				{
					break;
				}
			}
	
			iResult = send(ConnectSocket.socket_instance, sendbuff, strlen(sendbuff), 0);
			if (iResult == SOCKET_ERROR)
			{
				std::cout << "Send failed" << std::endl;
				return 1;
			}
			memset(&sendbuff, 0, sizeof(sendbuff));

		}
	
		
	}
		//shutdown the connection for sending, but still allow the receiving of data
		iResult = shutdown(ConnectSocket.socket_instance, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "Shutdown failed" << std::endl;
			return 1;
		}


		////receive data
		//do
		//{
		//	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		//	if (iResult > 0)
		//	{
		//		std::cout << iResult << " Bytes received" << std::endl;
		//	}
		//	else if (iResult == 0)
		//	{
		//		std::cout << "Connection closed" << std::endl;
		//	}
		//	else
		//	{
		//		std::cout << "Recv failed" << std::endl;
		//	}

		//} while (iResult > 0);
	
	//end
	return 0;

}
