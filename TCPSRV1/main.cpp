// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "..\shared\network_wrapper.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


#define DEFAULT_PORT "1337"
#define DEFAULT_BUFLEN 1024

//global lock
std::mutex socket_lock;

//void accept_connections(Net_socket &listen_socket)
//{
//	while (true)
//	{
//		Net_socket client_socket{};
//		client_socket.socket_instance = accept(listen_socket.socket_instance, NULL, NULL);
//		if (client_socket.socket_instance == INVALID_SOCKET)
//		{
//			std::cout << "accepting a connection failed" << std::endl;
//		}
//		else
//		{
//			
//			std::thread process_thread(process_connection, std::ref(client_socket));
//			process_thread.detach();
//
//		}
//	}
//}

void process_connection(Net_socket &&socket)
{

	while (true)
	{

		////////////////////////////////////////////
		//send and receive data
		char recvbuf[DEFAULT_BUFLEN]{};
		int iSendResult{};
		int recvbuflen = DEFAULT_BUFLEN;


		int recv_result = recv(socket.socket_instance, recvbuf, recvbuflen - 1, 0);
		

		if (recv_result > 0)
		{
			std::cout << recv_result << " Bytes received" << std::endl;
			recvbuf[recv_result] = '\0';
			std::cout << "Received data is : " << recvbuf << std::endl;


			//zero out the buff
			memset(&recvbuf, 0, sizeof(recvbuf));
		}

		else if (recv_result == SOCKET_ERROR)
		{
			std::cout << "ERROR receiving data" << std::endl;
		}

	}
}


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

	///
	///Create mutex to make the vector thread safe
	/// 
	
	//std::mutex socket_mutex;


	/// 
	////////////////////////////////////////////
	//accept connections on the socket
	
	//std::vector<Net_socket> client_sockets{};
	
	bool accept_connection = true; 
	
	//std::thread accept_thread(accept_connections, std::ref(listen_socket));
	//accept_thread.detach();

	while (true)
	{
		Net_socket client_socket{};
		client_socket.socket_instance = accept(listen_socket.socket_instance, NULL, NULL);

		/////temp
		//char recvbuf[DEFAULT_BUFLEN]{};
		//int iSendResult{};
		//int recvbuflen = DEFAULT_BUFLEN;


		//int recv_result = recv(client_socket.socket_instance, recvbuf, recvbuflen - 1, 0);
		//////
		//if (recv_result > 0)
		//{
		//	std::cout << iResult << " Bytes received" << std::endl;
		//	recvbuf[iResult] = '\0';
		//	std::cout << "Received data is : " << recvbuf << std::endl;
		//	memset(&recvbuf, 0, sizeof(recvbuf));
		//}

		if (client_socket.socket_instance == INVALID_SOCKET)
		{
			std::cout << "accepting a connection failed" << std::endl;
		}
		else
		{
			std::thread process_thread(process_connection, std::move(client_socket));
			process_thread.detach();
			//works when I use .join()
			//process_thread.join();
			//also works when I use the normal function, so I know the issue is the thread
			//process_connection(std::move(client_socket));
		}
	}

	return 0;
}