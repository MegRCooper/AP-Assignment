// Adapted from http://www.linuxhowtos.org/C_C++/socket.htm
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>

#include "networkInp.h"

#define MAX_BUFFER_SIZE 25000
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

/**
	The mutex class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads.
**/

class Client {
	int lstnSocket;
	struct sockaddr_in serverAddr;
	int server;
	char buffer[MAX_BUFFER_SIZE];
	mutex mutx;
	condition_variable conVar;
	int DEF_PORT;
	struct addrinfo* result = NULL, * pointer = NULL, hints;
	int intResult;
	SOCKET connectSocket = INVALID_SOCKET;
	WSADATA wsaData;
	networkInp* netInp;

public:
	string message = "Hello World.";
	void msg (const char* message) ;
	void start (const char* IPAddress, const char* port, networkInp* networkInput);
	void sendMsg ();
	void getMsg ();
	void close();
};