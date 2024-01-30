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
#define DEFAULT_BUFLEN 512



int main()
{
	////////////////////////////////////////////
	//INITIALIZE A SOCKET
	//init the winsock by creating a WSADATA obj
	WSADATA wsaData{};
	int iResult{};
	//call WSAstartup and check the result
	//request ver 2.2 of winsock be setup at address of wsaData
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
		return 1;
	}


	////////////////////////////////////////////
	//dns resolution
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	//setting the AI_PASSIVE flag here means we'll bind a socket
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		WSACleanup();
		std::cout << "getaddrinfo failed" << std::endl;
		return 1;
	}

	////////////////////////////////////////////
	//create an IPv4 TCP socket object
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		std::cout << "ListenSocket failed" << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	//at this point the socket is created but not bound
	
	////////////////////////////////////////////
	//bind the socket

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed" << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//getaddrinfo is no longer needed once the bind is complete
	freeaddrinfo(result);


	////////////////////////////////////////////
	//listen for incoming requests 

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed" << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	////////////////////////////////////////////
	//accept connections on the socket
	SOCKET ClientSocket = INVALID_SOCKET;
	
	//thread will listen here for a connection, no loop required for a simple example
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "accepting a connection failed" << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//no longer need the server socket because??
	closesocket(ListenSocket);

	////////////////////////////////////////////
	//send and receive data
	char recvbuf[DEFAULT_BUFLEN]{};
	int iSendResult{};
	int recvbuflen = DEFAULT_BUFLEN;

	do
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << iResult << " Bytes received" << std::endl;

			std::cout << "Received data is : " << recvbuf << std::endl;

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("Send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

			std::cout << iSendResult << " Bytes sent" << std::endl;
		}
		else if (iResult == 0)
		{
			std::cout << "Closing connection" << std::endl;
		}
		else
		{
			std::cout << "Receive failed" << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);


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

	//cleanup
	closesocket(ClientSocket);
	WSACleanup();



	return 0;
}