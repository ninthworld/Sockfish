#pragma once

#include <iostream>
#include <windows.h>
#include <ctype.h>
#include <string>
#include <vector>
#include "bitboards.h"

struct Move {
	unsigned int from;
	unsigned int to;
};

namespace CLI {

	void init();
	void loop();
	void printBoard();
	bool parseMove(std::string str, Move *move);
	Move getMove();
}