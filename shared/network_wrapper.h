#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

//
#include <iostream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "1337"
#define DEFAULT_BUFLEN 1024

struct Addr_info
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	//default ctor
	Addr_info()
	{
		ZeroMemory(&hints, sizeof(hints));
	}

	//dtor
	~Addr_info()
	{
		freeaddrinfo(result);
	}

	//deleted copy ctor
	Addr_info(const Addr_info &other) = delete;

	//deleted move ctor
	Addr_info(Addr_info &&other) noexcept = delete;

	//deleted copy assignment
	Addr_info &operator=(const Addr_info &other) = delete;

	//deleted move assignment
	Addr_info &operator=(Addr_info &&other) noexcept = delete;


};

struct Wsa_data
{
	int result = 0;
	WSADATA wsaData{};

	//default ctor
	Wsa_data()
	{
		//request ver 2.2 of winsock be setup at address of wsaData		
		result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			std::cout << "WSAStartup failed" << std::endl;
		}
	}

	//dtor
	~Wsa_data()
	{
		WSACleanup();
	}

	//copy ctor
	Wsa_data(const Wsa_data &other) = delete;

	//move ctor
	Wsa_data(Wsa_data &&other) noexcept = default;

	//copy assignment
	Wsa_data &operator=(const Wsa_data &other) = delete;

	//move assignment
	Wsa_data &operator=(Wsa_data &&other) noexcept = default;
};


struct Net_socket
{
	//DWORD bytes_in_buffer = 0;
	//DWORD flags = 0;
	SOCKET socket_instance = INVALID_SOCKET;
	//char wsaRawBuf[DEFAULT_BUFLEN]{};
	//WSABUF wsaBuf{ DEFAULT_BUFLEN, wsaRawBuf };

	//default ctor
	Net_socket() = default;

	//param ctor
	Net_socket(Addr_info &name_hint)
	{
		socket_instance = socket(name_hint.result->ai_family, name_hint.result->ai_socktype, name_hint.result->ai_protocol);
	}

	//dtor
	~Net_socket()
	{
		closesocket(socket_instance);
	}

	//copy ctor
	Net_socket(const Net_socket &other) = delete;

	//move ctor
	Net_socket(Net_socket &&other) noexcept = default;


	//copy assignment
	Net_socket &operator=(const Net_socket &other) = delete;

	//move assignment
	Net_socket &operator=(Net_socket &&other) noexcept = default;
};