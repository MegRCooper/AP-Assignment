#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include<sstream>
#include <iostream>
#include <sys/types.h>

#include <mutex>
#include "threadPool.h"
#include "serverControl.h"

#pragma comment (lib, "Ws2_32.lib")
#define MAX_BUFFER_SIZE 250000
using namespace std;

namespace information {
	std::mutex mutx;
	int sockfd [MAX_NUM_CLIENTS];
	int newSockfd[MAX_NUM_CLIENTS];
}

class Server {
	struct sockaddr_in serverAddr, clientAddr;
	struct hostend *server;
	char buffer[MAX_BUFFER_SIZE];
	int portNum;
	int clientID;
	WSADATA wsaData;
	int iResult;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	serverControl* serverCon;
public:
	Server(int, int, serverControl &con);
	void msg(const char*);
	void connect();
	void interact();
	string getData();
};

Server::Server(int port, int i, serverControl &con) {
	this->portNum = port + i;
	this->clientID = i;
	this->serverCon = &con;
}

void Server::msg(const char* text) {
	closesocket(information::newSockfd[clientID]);
	closesocket(information::sockfd[clientID]);
	perror(text);
	exit(0);
}

void Server::connect() {
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with Error: %d\n", iResult);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, to_string(portNum).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("Getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
	}

	information::sockfd[clientID] = socket(AF_INET, SOCK_STREAM, 0);
	if (information::sockfd[clientID] < 0)
		msg("ERROR opening socket");

	memset((char*)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(portNum);
	if (::bind(information::sockfd[clientID], (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		msg("ERROR on binding");

	listen(information::sockfd[clientID], 5);
	socklen_t clilen = sizeof(clientAddr);
	information::newSockfd[clientID] = accept(information::sockfd[clientID], (struct sockaddr*)&clientAddr, &clilen);
	if (information::newSockfd[clientID] < 0)
		msg("ERROR on accept");
	else
		cout << "\nClient " << portNum << " accepted!\n" << endl;
	// where to put the add player
	serverCon->addPlayer(clientID);
	if (serverCon->getSendID(clientID) != clientID) { // then you are player 2
		send(information::newSockfd[clientID], "C2\n", 3, 0);
		send(information::newSockfd[serverCon->getSendID(clientID)], "N\n", 3, 0);
	}
	else {
		send(information::newSockfd[clientID], "C1\n", 3, 0);
	}
	for (int i = 0; i < serverCon->currntPlyrCnt; i++) {
		send(information::newSockfd[i], ("\n M " + to_string(serverCon->sheetID[serverCon->currntPlyrCnt - 1]) + "\n").c_str(), 64, 0);
	}
}

void Server::interact() {
	while (true) {
		cout << "\nListening . . .\n" << endl;
		string data = " "; 
		data += this->getData();
		int n;
		string reply = "";
		for (int i = 0; i < serverCon->currntPlyrCnt; i++) {
			reply = "";
			if (i != this->clientID) {
				if (data[0] == 'S' || data[0] == 'F') {
					reply += std::to_string(this->clientID);
				}
				else {
					reply += std::to_string(serverCon->getSheetID(serverCon->getSendID(i), serverCon->sheetID[serverCon->getSendID(this->clientID)]));
				}
				reply += " ";
				reply += data;
				reply += "\n";
				n = send(information::newSockfd[i], reply.c_str(), reply.size(), 0);
			}
			else if (data[0] == 'S' || data[0] == 'F' || this->clientID == serverCon->getSendID(this->clientID)) {
				if ((data[0] == 'P') || (data[0] == 'B')) {
					reply += "I";
				}
				reply += std::to_string(i);
				reply += " ";
				reply += data;
				reply += "\n";
				n = send(information::newSockfd[i], reply.c_str(), reply.size(), 0);
			}
		}
		if (n < 0) msg("\n ERROR writing to Socket \n");
	}
}

string Server::getData() {
	int n = 0;
	string fullBuffer = " ";
	while (true) {
		memset(buffer, 0, MAX_BUFFER_SIZE);
		int n = recv(information::newSockfd[clientID], buffer, MAX_BUFFER_SIZE - 1, 0);
		if (n < 0) msg("\n ERROR reading from socket \n");
		fullBuffer = fullBuffer + buffer;
		if (strlen(buffer) == 0) msg("\n ERROR empty buffer \n");
		if (buffer[strlen(buffer) - 1] == ';' || n == 0) break;
		break;
	}
	cout << "\n Retrieved data from port number: " << portNum << "\n";
	return (fullBuffer);
}

int main(int argc, const char* argv[]) {
	if (argc < 1) {
		cout << "\nUsage: " << argv[0] << " port" << endl;
	}
	else {
		string portIn;
		cout << "\nEnter a default port, other ports will be opened incrementing this port number" << endl;
		cin >> portIn;
		serverControl head = serverControl();
		std::vector<Server> servers;
		ThreadPool threads(MAX_NUM_CLIENTS);
		int port = atoi(portIn.c_str());
		for (int i = 0; i < MAX_NUM_CLIENTS; i++) {
			threads.enqueue([port, i, &head] {
				cout << "\nStarting client port " << (port + i) << endl;
				Server server = Server(port, i, head);
				server.connect();
				server.interact();
				});
		}
		cout << "\n LAUNCHED . . ." << endl;
	}
	return 0;
}