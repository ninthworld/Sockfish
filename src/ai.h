#pragma once

#include <algorithm>
#include "moves.h"
#include "bitboards.h"

const int W_MINI_NINJA_SCORE = -15;
const int W_MINI_SAMURAI_SCORE = -10;
const int W_NINJA_SCORE = -40;
const int W_SAMURAI_SCORE = -30;
const int W_KING_SCORE = -999;

const int R_MINI_NINJA_SCORE = 15;
const int R_MINI_SAMURAI_SCORE = 10;
const int R_NINJA_SCORE = 40;
const int R_SAMURAI_SCORE = 30;
const int R_KING_SCORE = 999;

namespace AI {

	Move getBestMove(Board board);

	int minimax(Board board, PieceColor aiColor, int depth);

	int getBoardScore(Board board);
}