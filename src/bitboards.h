#pragma once

#include <cstdint>
#include "types.h"

#define ROWS 8
#define COLS 7

typedef uint64_t Bitboard;

extern Bitboard board[COLOR_NB][PIECE_TYPE_NB];

namespace Bitboards {

	void init();

	bool getPieceAt(int col, int row, Color *color, PieceType *piece);
}