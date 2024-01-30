#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


#include <iostream>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "1337"
#define DEFAULT_BUFLEN 512



int main(int argc, char *argv[])
{
	WSADATA wsaData{};
	struct addrinfo *result = NULL, *ptr = NULL, hints;


	////validate
	//if (argc != 2)
	//{
	//	std::cout << "need a server IP or name" << std::endl;
	//	return 1;
	//}

	//init winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
		return 1;
	}


	ZeroMemory(&hints, sizeof(hints));
	//sets the l3 proto as IPv4
	hints.ai_family = AF_INET;
	//sets the l4 proto to tcp, requires IPPROTO_TCP as the ai_protocol
	hints.ai_socktype = SOCK_STREAM;
	//sets the l4 proto to tcp
	hints.ai_protocol = IPPROTO_TCP;

	//we use the hints to select an adapter
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed" << std::endl;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	//set the socket up to the first IP learned by getaddrinfo
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	//check for errors
	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Error creating the socket" << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//////////////
	//connect to the tcp server
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	//no longer need the result object since we select our interface and IP
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Unable to connect to server" << std::endl;
		WSACleanup;
		return 1;
	}

	//setup buffers for send and receive
	int recvbuflen = DEFAULT_BUFLEN;
	const char *sendbuf = "This is a test";
	char recvbuf[DEFAULT_BUFLEN];

	//send data
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Send failed" << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	std::cout << (int)strlen(sendbuf) << " Bytes sent" << std::endl;

	//shutdown the connection for sending, but still allow the receiving of data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Shutdown failed" << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}


	//receive data
	do
	{
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << iResult << " Bytes received" << std::endl;
		}
		else if (iResult == 0)
		{
			std::cout << "Connection closed" << std::endl;
		}
		else
		{
			std::cout << "Recv failed" << std::endl;
		}
	
	} while (iResult > 0);


	//cleanup
	closesocket(ConnectSocket);
	WSACleanup();



	//end
	return 0;

}
