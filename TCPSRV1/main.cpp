// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <vector>

#include "..\shared\network_wrapper.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


#define DEFAULT_PORT "1337"
#define DEFAULT_BUFLEN 1024


int main()
{
	////////////////////////////////////////////
	//INITIALIZE A SOCKET
	//init the winsock by creating a WSADATA obj
	Wsa_data wsaData{};
	int iResult = 0;

	////////////////////////////////////////////
	//choosing the correct adapter and Ip address

	Addr_info name_hint{};
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
	Net_socket listen_socket(name_hint);
	if (listen_socket.socket_instance == INVALID_SOCKET)
	{
		std::cout << "listen_socket failed" << std::endl;
		return 1;
	}
	//at this point the socket is created but not bound
	
	////////////////////////////////////////////
	//bind the socket

	iResult = bind(listen_socket.socket_instance, name_hint.result->ai_addr, (int)name_hint.result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed" << std::endl;
		return 1;
	}

	////////////////////////////////////////////
	//listen for incoming requests 

	if (listen(listen_socket.socket_instance, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed" << std::endl;
		return 1;
	}


	
	////////////////////////////////////////////
	//accept connections on the socket
	Net_socket client_socket{};
	std::vector<Net_socket> client_sockets{};
	
	bool accept_connections = true;
	
	
	
	//thread will listen here for a connection, no loop required for a simple example
	client_socket.socket_instance = accept(listen_socket.socket_instance, NULL, NULL);

	client_sockets.push_back(std::move(client_socket));

	if (client_sockets.back().socket_instance == INVALID_SOCKET)
	{
		std::cout << "accepting a connection failed" << std::endl;
		return 1;
	}


	while (accept_connections)
	{
		const TIMEVAL socket_timeout{ 1, 0 };
		fd_set socket_set{ client_sockets.size(), listen_socket.socket_instance };
		int socket_check_result = select(0, &socket_set, NULL, NULL, &socket_timeout);

		if (socket_check_result)
		{
			Net_socket new_client_socket{};
			new_client_socket.socket_instance = accept(listen_socket.socket_instance, NULL, NULL);
			//thread will listen here for a connection, no loop required for a simple example
			if (new_client_socket.socket_instance == INVALID_SOCKET)
			{
				std::cout << "accepting a connection failed" << std::endl;
				return 1;
			}
			client_sockets.push_back(std::move(new_client_socket));
		}

		for (Net_socket &sock : client_sockets)
		{


			//no longer need the server socket because??
		//closesocket(listen_socket);

		////////////////////////////////////////////
		//send and receive data
			char recvbuf[DEFAULT_BUFLEN]{};
			int iSendResult{};
			int recvbuflen = DEFAULT_BUFLEN;

			//bool receive_data = true;
			//while (receive_data)
			//{
				iResult = recv(sock.socket_instance, recvbuf, recvbuflen - 1, 0);
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
				//}
				//else
				//{
				//	receive_data = false;
				//}
				//zero out the buff
				memset(&recvbuf, 0, sizeof(recvbuf));
			}

			//if (iResult == 0)
			//{
			//	std::cout << "Closing connection" << std::endl;
			//	////////////////////////////////////////////
			//	//begin shutting down the connection, only allow data to be received not sent

			//	iResult = shutdown(sock.socket_instance, SD_SEND);
			//	if (iResult == SOCKET_ERROR)
			//	{
			//		std::cout << "Shutdown failed" << std::endl;
			//		return 1;
			//	}

			//}
			//else
			//{
			//	std::cout << "Receive failed" << std::endl;
			//	return 1;
			//}


		}

	}
	

	return 0;
}