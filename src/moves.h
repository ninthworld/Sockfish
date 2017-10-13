#pragma once

#include "bitboards.h"

#define MAX_STEPS 7

struct Move {
	Pos from;
	Pos to;
};

enum MoveDirection {
	UP, DOWN, LEFT, RIGHT,
	MOVE_DIRECTION_NB
};

enum MoveType {
	MOVE_NORMAL, MOVE_ATTACK,
	MOVE_TYPE_NB
}; 

namespace Moves {

	extern Bitboard genMovesBB[PIECE_COLOR_NB][PIECE_TYPE_NB][ROWS][COLS][MOVE_DIRECTION_NB][MOVE_TYPE_NB][MAX_STEPS];

	void init();

	Bitboard getGenMoves(PieceColor color, PieceType pieceType, Pos pos, MoveDirection dir, MoveType moveType, unsigned int step);
	void setGenMoves(PieceColor color, PieceType pieceType, Pos pos, MoveDirection dir, MoveType moveType, unsigned int step, Bitboard bb);

	std::vector<Move> getLegalMoves(Pos pos, PieceColor currentTurn, Bitboard *bb);
	bool isMoveLegal(Move move, PieceColor currentTurn);

	void doMove(Move move);
}