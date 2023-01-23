#pragma once

/**
	Player Class:
	This provides the class definition for the player, consisting of two members [score & index]
		score [int] - stores the current score, incremented everytime a point is won,
		indx [int] - tracks how many times the player has been created
	If index equals 0 then colour will be set to Red; otherwise it will be set to Yellow [trad Curling colours].
	PlayerColours enum type with two possible values; RED, YELLOW. **/

enum PlayersColours { RED, YELLOW };

class player {
	static int playerIndxCnt;
public:
	//int score;
	bool isCurrntTurn;
	int indx;
	//PlayersColours colour;

	//player() {
	//	indx = playerIndxCnt++;
	//	if (indx == 0) {
	//		colour = RED;
	//	}
	//	else {
	//		colour = YELLOW;
	//	}
	//	Reset();
	//}
	//void Reset(void);
};

/**
	Teams Class:
		The code declares a player class and an array of players.
**/
class teams {
public:
	// player players[MAX_PLAYERS];
	int currntNumPlayers = 0;
	int currntScore = 0;
};


class player {
	static int playerIndex;
public:
	int score;
	int index;
	player() : score(0) {
		index = playerIndex++;
	}
};