#pragma once

#include "types.h"
#include "bitboards.h"

class Game {
public:
	Game(PieceColor first);

	Board getBoard();
	void setBoard(Board board);

	int getTurn();

private:
	Board board;
};