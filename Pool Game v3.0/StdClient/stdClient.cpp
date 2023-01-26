#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// Link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// Max length of buffer (default).
#define DEFAULT_BUFF 512
// Default port num:
#define DEFAULT_PORT "800"

int __cdecl main(int argC, char** argV) {
	WSADATA wsaData;
	SOCKET cSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	char* sendBuffer = (char*)"Test: Hello World";
	char recvBuffer[DEFAULT_BUFF];
	int intResult;
	int recvBufferLen = DEFAULT_BUFF;

	// Parameter Validation:
	if (argC != 2) {
		printf("Usage: %s serverName\n", argV[0]);
		return 1;
	}

	// Initialise Winsock.
	intResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (intResult != 0) {
		printf("WSAStartup failed with an error, code: %d\n", intResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port.
	intResult = getaddrinfo(argV[1], DEFAULT_PORT, &hints, &result);
	if (intResult != 0) {
		printf("getaddrinfo failed with an error, code: %d\n", intResult);
		WSACleanup();
		return 1;
	}

	// Connection of an address, attepmts until success.
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		cSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (cSocket == INVALID_SOCKET) {
			printf("Socket failed error, code: %d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		// Connection to the server.
		intResult = connect(cSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (intResult == SOCKET_ERROR) {
			closesocket(cSocket);
			cSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (cSocket == INVALID_SOCKET) {
		printf("UNABLE to connect to server!\n");
		WSACleanup();
		return 1;
	}

	std::cout << "Still Connected: " << std::endl;
	while (true) {
		printf("Client: ");
		fgets(recvBuffer, 255, stdin);
		intResult = send(cSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (intResult == SOCKET_ERROR) {
			printf("Send failed error, code: %d\n", WSAGetLastError());
			closesocket(cSocket);
			WSACleanup();
			return 1;
		}
		intResult = recv(cSocket,recvBuffer,recvBufferLen,0);
		if (intResult > 0) {
			printf("Server says: %s\n", recvBuffer);
		}

	}
	// Clean up:
	closesocket(cSocket);
	WSACleanup();
	return 0;
}