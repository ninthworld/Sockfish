#pragma once

#include <cstdint>
#include <vector>
#include "types.h"

#define ROWS 8
#define COLS 7

typedef uint64_t Bitboard;

struct Pos {
	int row;
	int col;
};

Pos getPos(Bitboard bb);
int getPosIndex(Pos pos);

namespace Bitboards {

	extern Bitboard boardBB[PIECE_COLOR_NB][PIECE_TYPE_NB];

	void init();

	Piece getPieceAt(Pos pos);

	Bitboard getBoard(Piece piece);
	Bitboard getBoardColor(PieceColor color);

	void setBoard(Piece piece, Bitboard bb);
}