// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


#define DEFAULT_PORT "1337"
#define DEFAULT_BUFLEN 12

class addr_info
{
	public:
		struct addrinfo *result = NULL, *ptr = NULL, hints;

		addr_info()
		{
			ZeroMemory(&hints, sizeof(hints));
		}
		~addr_info()
		{
			freeaddrinfo(result);
		}
	private:
};

class wsa_data
{
	public:
		int result = 0;
		WSADATA wsaData{};

		wsa_data()
		{
			//request ver 2.2 of winsock be setup at address of wsaData		
			result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		}

		~wsa_data()
		{
			WSACleanup();
		}
		
	private:
};


class net_socket
{
	public:
		SOCKET socket_instance = INVALID_SOCKET;

		net_socket() = default;
		net_socket(addr_info &name_hint)
		{
			socket_instance = socket(name_hint.result->ai_family, name_hint.result->ai_socktype, name_hint.result->ai_protocol);
		}
		~net_socket()
		{
			closesocket(socket_instance);
		}

	private:
};

int main()
{
	////////////////////////////////////////////
	//INITIALIZE A SOCKET
	//init the winsock by creating a WSADATA obj
	wsa_data wsaData{};
	int iResult = 0;


	if (wsaData.result != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
		return 1;
	}


	////////////////////////////////////////////
	//chgoosing the correct adapter and Ip address
	/////////*struct addrinfo *result = NULL, *ptr = NULL, hints;
	////////ZeroMemory(&hints, sizeof(hints));*/
	addr_info name_hint{};
	name_hint.hints.ai_family = AF_INET;
	name_hint.hints.ai_socktype = SOCK_STREAM;
	name_hint.hints.ai_protocol = IPPROTO_TCP;
	//setting the AI_PASSIVE flag here means we'll bind a socket
	name_hint.hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &name_hint.hints, &name_hint.result);
	if (iResult != 0)
	{
		
		std::cout << "getaddrinfo failed" << std::endl;
		return 1;
	}

	////////////////////////////////////////////
	//create an IPv4 TCP socket object
	//SOCKET ListenSocket = INVALID_SOCKET;
	net_socket ListenSocket(name_hint);
	//ListenSocket = socket(name_hint.result->ai_family, name_hint.result->ai_socktype, name_hint.result->ai_protocol);
	if (ListenSocket.socket_instance == INVALID_SOCKET)
	{
		std::cout << "ListenSocket failed" << std::endl;
		return 1;
	}
	//at this point the socket is created but not bound
	
	////////////////////////////////////////////
	//bind the socket

	iResult = bind(ListenSocket.socket_instance, name_hint.result->ai_addr, (int)name_hint.result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed" << std::endl;
		return 1;
	}

	//getaddrinfo is no longer needed once the bind is complete
	//freeaddrinfo(name_hint.result);


	////////////////////////////////////////////
	//listen for incoming requests 

	if (listen(ListenSocket.socket_instance, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed" << std::endl;
		return 1;
	}

	////////////////////////////////////////////
	//accept connections on the socket
	SOCKET ClientSocket = INVALID_SOCKET;
	
	//thread will listen here for a connection, no loop required for a simple example
	ClientSocket = accept(ListenSocket.socket_instance, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "accepting a connection failed" << std::endl;
		return 1;
	}

	//no longer need the server socket because??
	//closesocket(ListenSocket);

	////////////////////////////////////////////
	//send and receive data
	char recvbuf[DEFAULT_BUFLEN]{};
	int iSendResult{};
	int recvbuflen = DEFAULT_BUFLEN;

	bool receive_data = true;
		while (receive_data)
		{
			iResult = recv(ClientSocket, recvbuf, recvbuflen - 1, 0);
			if (iResult > 0)
			{
				std::cout << iResult << " Bytes received" << std::endl;
				recvbuf[iResult] = '\0';
				std::cout << "Received data is : " << recvbuf << std::endl;

				//// Echo the buffer back to the sender
				//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
				//if (iSendResult == SOCKET_ERROR) {
				//	printf("Send failed with error: %d\n", WSAGetLastError());
				//	closesocket(ClientSocket);
				//	WSACleanup();
				//	return 1;
				//}

				//std::cout << iSendResult << " Bytes sent" << std::endl;
			}
			//zero out the buff
			memset(&recvbuf, 0, sizeof(recvbuf));
		}

		if (iResult == 0)
		{
			std::cout << "Closing connection" << std::endl;
			////////////////////////////////////////////
			//begin shutting down the connection, only allow data to be received not sent

			iResult = shutdown(ClientSocket, SD_SEND);
			if (iResult == SOCKET_ERROR)
			{
				std::cout << "Shutdown failed" << std::endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

		}
		else
		{
			std::cout << "Receive failed" << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}


	//cleanup
	closesocket(ClientSocket);
	WSACleanup();



	return 0;
}