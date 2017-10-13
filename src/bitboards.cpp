#include "bitboards.h"

Pos getPos(Bitboard bb) {
	int index = 1;
	while(bb >>= 1) index++;
	return { (int) floor(index / COLS), (index - 1) % COLS };
}

int getPosIndex(Pos pos) {
	return pos.row * COLS + pos.col;
}

Bitboard Bitboards::boardBB[PIECE_COLOR_NB][PIECE_TYPE_NB];

void Bitboards::init() {
	boardBB[WHITE][NO_PIECE_TYPE]	= 0x0ULL;
	boardBB[WHITE][MINI_NINJA]		= 0x1c000ULL;
	boardBB[WHITE][MINI_SAMURAI]	= 0x1c0000ULL;
	boardBB[WHITE][NINJA]			= 0x3800ULL;
	boardBB[WHITE][SAMURAI]			= 0x380ULL;
	boardBB[WHITE][KING]			= 0x8ULL;

	boardBB[RED][NO_PIECE_TYPE]		= 0x0ULL;
	boardBB[RED][MINI_NINJA]		= 0x38000000000ULL;
	boardBB[RED][MINI_SAMURAI]		= 0x3800000000ULL;
	boardBB[RED][NINJA]				= 0x1c0000000000ULL;
	boardBB[RED][SAMURAI]			= 0x1c00000000000ULL;
	boardBB[RED][KING]				= 0x10000000000000ULL;
}

Piece Bitboards::getPieceAt(Pos pos) {
	Bitboard selector = 0x1ULL << (pos.col + pos.row * COLS);

	for (unsigned int i = WHITE; i < PIECE_COLOR_NB; ++i) {
		for (unsigned int j = MINI_NINJA; j < PIECE_TYPE_NB; ++j) {
			if (boardBB[i][j] & selector) {
				return { (PieceColor)i, (PieceType)j };
			}
		}
	}

	return { NO_PIECE_COLOR, NO_PIECE_TYPE };
}

Bitboard Bitboards::getBoard(Piece piece) {
	return boardBB[piece.color][piece.type];
}

Bitboard Bitboards::getBoardColor(PieceColor color) {
	Bitboard bb = 0x0ULL;

	for (unsigned int i = MINI_NINJA; i < PIECE_TYPE_NB; ++i) {
		bb |= getBoard({ color, (PieceType)i });
	}

	return bb;
}

void Bitboards::setBoard(Piece piece, Bitboard bb) {
	boardBB[piece.color][piece.type] = bb;
}