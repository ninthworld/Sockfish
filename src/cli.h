#pragma once

#include <iostream>
#include <windows.h>
#include <ctype.h>
#include <string>
#include <vector>
#include "bitboards.h"
#include "moves.h"
#include "game.h"

namespace CLI {

	PieceColor init();

	void loop(Game *game);
	Move getMove(PieceColor currentTurn);
	
	void printBoard(PieceColor currentTurn, unsigned int turnNum);
	void printBitboard(std::vector<Move> moves, Bitboard bitboard);

	std::string encodePos(Pos pos);
	std::string encodeMove(Move move);
	Pos decodePos(std::string str);
	Move decodeMove(std::string str);
}