#include "serverControl.h"
#include <algorithm>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

serverControl::serverControl() {
	currntPlyrCnt = 0;
	std::fill_n(playerIDs, MAX_NUM_CLIENTS, -1);
	std::fill_n(oppIDs, MAX_NUM_CLIENTS, -1);
}

void serverControl::addPlayer(int ID) {
	playerIDs[currntPlyrCnt] = ID;
	if (currntPlyrCnt % 2 == 1) {
		oppIDs[currntPlyrCnt - 1] = ID;
		oppIDs[currntPlyrCnt] = playerIDs[currntPlyrCnt - 1];
		sheetID[currntPlyrCnt] = (currntPlyrCnt - 1) / 2;
	}
	else {
		sheetID[currntPlyrCnt] = currntPlyrCnt / 2;
	}
	currntPlyrCnt++;
}

int serverControl::getSendID(int i) {
	int* index = std::find(playerIDs, playerIDs+ MAX_NUM_CLIENTS, i);
	if (oppIDs[*index] == -1) {
		return (playerIDs[*index]);
	}
	else {
		return (oppIDs[*index]);
	}
}

int serverControl::getSheetID(int sendID, int realSheetID) {
	if (sheetID[(sendID)] == 0) {
		return (realSheetID);
	}
	else {
		if (sheetID[(sendID)] == realSheetID) {
			return (0);
		}
		else if (realSheetID == 0) {
			return (sheetID[(sendID)]);
		}
		else {
			return (realSheetID);
		}
	}
	return (0);
}