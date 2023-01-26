// Standard Server code:
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

// Link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

// Max length of buffer (default).
#define DEFAULT_BUFF 512
// Default port num:
#define DEFAULT_PORT "800"

/**
	__cdecl is the default calling convention for C and C++ programs. Because the stack is cleaned up by the caller, 
	it can do vararg functions. The __cdecl calling convention creates larger executables than  __stdcall, because 
	it requires each function call to include stack cleanup code
**/
int __cdecl main(void) {
	/**
		WSADATA - contains info about the Windows Socket info.
	**/
	WSADATA wsaData;
	int intResult;

	SOCKET lSocket = INVALID_SOCKET;
	SOCKET cSocket = INVALID_SOCKET;

	// Description: The addrinfo structure describes address information for use with TCP/IP. To get this information, call getaddrinfo(); 
	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int intSendResults;
	// The recv function (winsock.h) receives data from a connected socket or a bound connectionless socket
	char recvBuffer[DEFAULT_BUFF];
	int recvBufferLen = DEFAULT_BUFF;

	// Initialise WinSock:
	intResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (intResult != 0) {
		printf("WSAStartup failed with an error, code: %d\n", intResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolves the server address and port.
	intResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (intResult != 0) {
		printf("getaddrinfo failed with an error, code: %d\n", intResult);
		WSACleanup();
		return 1;
	}

	// Creates a SOCKET (connecting to a server):
	lSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (lSocket == INVALID_SOCKET) {
		printf("Socket failed error, code: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setting up the TCP listening socket:
	intResult = bind(lSocket, result->ai_addr, (int)result->ai_addrlen);
	if (intResult == SOCKET_ERROR) {
		printf("Bind failed error, code: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(lSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);
	intResult = listen(lSocket, SOMAXCONN);
	if (intResult == SOCKET_ERROR) {
		printf("Listen failed with error, code: %d\n", WSAGetLastError());
		closesocket(lSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket.
	cSocket = accept(lSocket, NULL, NULL);
	if (cSocket == INVALID_SOCKET) {
		printf("Acceptance of client failed with error, code: %d\n", WSAGetLastError());
		closesocket(lSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer shutsdown the connection.
	do {
		intResult = recv(cSocket, recvBuffer, recvBufferLen, 0);
		if (intResult > 0) {
			printf("Client says: %s\n", recvBuffer);
			// Echos the buffer back to the sender.
			std::string serverReply = "Did you say ";
			serverReply += recvBuffer;
			serverReply += "'?";
			intSendResults = send(cSocket, serverReply.c_str(), intResult, 0);
			if (intSendResults == SOCKET_ERROR) {
				printf("Send failed with error, code: %d\n", WSAGetLastError());
				closesocket(cSocket);
				WSACleanup();
				return 1;
			}
		}
		else if (intResult == 0)
			printf ("Connection closing ...\n");
		else {
			printf("RECV failed with error, code: %d\n", WSAGetLastError());
			closesocket(cSocket);
			WSACleanup();
			return 1;
		}
	}
	while(intResult > 0);

	// Since we're done, need to close down the connection.
	intResult = shutdown(cSocket, SD_SEND);
	if (intResult == SOCKET_ERROR) {
		printf("Shutdown failed with error, code: %d\n", WSAGetLastError());
		closesocket(cSocket);
		WSACleanup();
		return 1;
	}
	// Clean up
	closesocket(cSocket);
	WSACleanup();
	return 0;
}