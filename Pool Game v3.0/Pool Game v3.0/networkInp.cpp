#include "networkInp.h"

void networkInp::DisplayScores(void) {
	std::cout << "----------------------------" << std::endl;
	for (int i = 0; i < SHEETCOUNT * 2; i++) {
		std::cout << "Player " << (i + 1) << ": " << scores[i] << std::endl;
	}
	std::cout << "----------------------------" << std::endl;
}
