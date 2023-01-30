#pragma once

#include "vecmath.h"
#include <string.h>
#include <iostream>
#include <sstream>

#define	SHEETCOUNT (5)

class networkInp{
public:
	vec2 stoneImps[SHEETCOUNT];
	float stonePos[SHEETCOUNT];
	int scores[SHEETCOUNT];
	bool impChange[SHEETCOUNT];
	bool posChange[SHEETCOUNT];
	bool finished[SHEETCOUNT];
	int maxSheets = 1;
	bool mainPlayer = true;
	bool sheetFull;
	bool updateScores;
	bool welcomeRecieved = false;

	void DisplayScores(void);
};