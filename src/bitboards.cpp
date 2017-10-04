#include "bitboards.h"

Bitboard board[COLOR_NB][PIECE_TYPE_NB];

void Bitboards::init() {
	board[WHITE][NO_PIECE_TYPE] = 0x0ULL;
	board[WHITE][MINI_NINJA]	= 0x1c000ULL;
	board[WHITE][MINI_SAMURAI]	= 0x1c0000ULL;
	board[WHITE][NINJA]			= 0x3800ULL;
	board[WHITE][SAMURAI]		= 0x380ULL;
	board[WHITE][KING]			= 0x8ULL;

	board[RED][NO_PIECE_TYPE]	= 0x0ULL;
	board[RED][MINI_NINJA]		= 0x38000000000ULL;
	board[RED][MINI_SAMURAI]	= 0x3800000000ULL;
	board[RED][NINJA]			= 0x1c0000000000ULL;
	board[RED][SAMURAI]			= 0x1c00000000000ULL;
	board[RED][KING]			= 0x10000000000000ULL;
}

bool Bitboards::getPieceAt(int col, int row, Color *color, PieceType *piece) {
	Bitboard selector = 0x1ULL << (col + row * COLS);

	for (unsigned int i = 0; i < COLOR_NB; ++i) {
		for (unsigned int j = 1; j < PIECE_TYPE_NB; ++j) {
			if (board[i][j] & selector) {
				*color = (Color) i;
				*piece = (PieceType) j;
				return true;
			}
		}
	}

	*color = WHITE;
	*piece = NO_PIECE_TYPE;

	return false;
}