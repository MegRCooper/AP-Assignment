#include "teams.h"

int player::IDcount = 0;

player::player() {
	ID = IDcount;
	IDcount++;
	doCue = false;
	name = "COO19695119";
}

std::vector<player*> team::activePlayers = {};

team::team() {
	players = {};
	activePlayers = {};
	for (int i = 0; i < 3; i++) {
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colour(i) = r;
	}
}

void team::AddPlayer(player* _player) {
	players.push_back(_player);
	activePlayers.push_back(_player);
}

void team::RemovePlayer(player* _player) {
	for (int i = activePlayers.size() - 1; i >= 0; i--) {
		if (players[i] == _player) {
			players.erase(players.begin() + i);
		}
	}

	for (int i = activePlayers.size() - 1; i >= 0; i--) {
		if (activePlayers[i] == _player) {
			activePlayers.erase(activePlayers.begin() + i);
		}
	}
}

void team::DeletePlayer(player* _player) {
	for (int i = players.size() - 1; i >= 0; i--) {
		if (players[i] == _player) {
			players.erase(players.begin() + i);
		}
	}

	for (int i = activePlayers.size() - 1; i >= 0; i--) {
		if (activePlayers[i] == _player) {
			activePlayers.erase(activePlayers.begin() + i);
		}
	}

	delete _player;
}