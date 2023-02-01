#include "serialisedThreadedClient.h"
#include "networkInp.h"

Client gClient;

void Client::msg(const char* message) {
	closesocket(lstnSocket);
	perror(message);
	exit(0);
};
	
void Client::start(const char* IPAddress, const char* port, networkInp* networkInput) {
	intResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (intResult != 0) {
		printf("WSAStartup failed with error: %d \n", intResult);
	}
	netInp = networkInput;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	intResult = getaddrinfo(IPAddress, port, &hints, &result);
	if (intResult != 0) {
		printf("Getaddrinfo failed with error: %d \n", intResult);
		WSACleanup();
	}

	// Attempts to connect to an address till success.
	for (pointer = result; pointer != NULL; pointer = pointer->ai_next) {
		// Creates a SOCKET for connecting to a server:
		connectSocket = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);
		if (connectSocket == INVALID_SOCKET) {
			printf("Socket failed with error: %d \n", WSAGetLastError());
			WSACleanup();
		}

		// Connect to a server.
		intResult = connect(connectSocket, pointer->ai_addr, (int)pointer->ai_addrlen);
		if (intResult == SOCKET_ERROR) {
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server! \n");
		WSACleanup();
	}
	
};

void Client::sendMsg() {
	int i = send(connectSocket, message.c_str(), strlen(message.c_str()), 0);
	if (i < 0) {
		msg("ERROR writing to socket");
	}
};

void Client::getMsg() {
	while (true) {
		string fullBuffer = " ";
		while (true) {
			ZeroMemory(buffer, MAX_BUFFER_SIZE);
			int n = recv(connectSocket, buffer, MAX_BUFFER_SIZE - 1, 0);
			if (n < 0) {
				cout << WSAGetLastError << endl;
				msg("ERROR reading from socket");
			}
			fullBuffer = fullBuffer + buffer;
			if (buffer[strlen(buffer) - 1] == '\n' || n == 0 || strlen(buffer) == 0) break;
		}

		if (fullBuffer.length() > 0 && (fullBuffer[0] != 'I')) { // 'I' is a placeholder, used to ignore message.
			if (fullBuffer[1] == 'C') {
				cout << "Connected to the server" << endl;
				if (fullBuffer[2] == '1') {
					netInp->mainPlayer = true;
				}
				else {
					netInp->mainPlayer = false;
					netInp->sheetFull = true;
				}
				netInp->welcomeRecieved = true;
			}
			// For is a new player joins the sheet.
			else if (fullBuffer[0] == 'N') {
				netInp->sheetFull = true;
			}

			if (fullBuffer[0] == 'M') {
				netInp->maxSheets = atoi(&fullBuffer[2]) + 1;
			}
			else {
				string ID = " ";
				bool numEnd = false;
				int i = 0;

				// Gets the ID of the message incase there are more than 40 players making the ID able to be more than 1 character
				while (!numEnd) {
					ID += fullBuffer[i];
					i += 1;
					if (fullBuffer[i] == ' ' || fullBuffer[i] == '\n') { numEnd = true; }
				}
				switch (fullBuffer[i + 1]) {
				case ('B'): // Receives impulse for the stones
					netInp->stoneImps[stoi(ID)](0) = stof(fullBuffer.substr(i + 3, i + 11));
					netInp->stoneImps[stoi(ID)](1) = stof(fullBuffer.substr(i + 13, i + 21));
					if (fullBuffer[13] == '_') {
						netInp->stoneImps[stoi(ID)](1) = -netInp->stoneImps[stoi(ID)](1);
					}
					netInp->impChange[stoi(ID)] = true;
					break;

				case ('P'): // Receives the position of the stones
					netInp->stonePos[stoi(ID)] = stof(fullBuffer.substr(i + 3, i + 11));
					netInp->posChange[stoi(ID)] = true;
					break;

				case ('S'): // Recieves the score
					netInp->scores[stoi(ID)] = stoi(fullBuffer.substr(i + 3, fullBuffer.size()));
					netInp->updateScores = true;
					break;

				case ('F'): // Checks if the player has finished their turn.
					netInp->finished[stoi(ID)] = true;
				default:
					break;
				}
			}
		}
	}
		conVar.notify_one();
};
	
void Client::close() {
	closesocket(connectSocket);
};