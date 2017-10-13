#pragma once

#include "types.h"

class Game {
public:
	Game(PieceColor firstColor);

	PieceColor getCurrentTurn();
	unsigned int getTurnNum();

	void nextTurn();

private:
	PieceColor currentTurn;
	unsigned int halfTurnNum;
	unsigned int turnNum;
};