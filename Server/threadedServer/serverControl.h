#pragma once
#define MAX_NUM_CLIENTS 40

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class serverControl {
	int playerIDs [MAX_NUM_CLIENTS];
	int oppIDs [MAX_NUM_CLIENTS];
public:
	int sheetID[MAX_NUM_CLIENTS];
	int currntPlyrCnt;
	serverControl();
	void addPlayer (int);
	int getSendID (int);
	int getSheetID(int, int);
};
