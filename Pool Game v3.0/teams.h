#pragma once
#include"vecmath.h"
#include <vector>
#include <string>

class player {
	static int IDCount;
private:
	int ID;
public:
	bool doAim;
	std::string name;
	char ip;

	player();
};

class team {
	// Vector of players to prevent duplication of players
	static std::vector<player*> activePlayers; 
public:
	std::string name;
	vec3 colour;
	std::vector<player*> players;

	team();
	void AddPlayer(player*);
	void RemovePlayer(player*);
	void DeletePlayer(player*);
};