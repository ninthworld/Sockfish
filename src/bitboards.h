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

struct Board {
	Bitboard piece[PIECE_COLOR_NB][PIECE_TYPE_NB];
	int pieceCount[PIECE_COLOR_NB][PIECE_TYPE_NB];
	PieceColor turn;
	int ply;
};

namespace Bitboards {

	extern Bitboard tileBB[ROWS][COLS];

	void init();
	Bitboard getTileAt(Pos pos);

	Board createBoard();

	Piece getPieceAt(Board board, Pos pos);

	Bitboard getBoardPiece(Board board, Piece piece);
	Bitboard getBoardColor(Board board, PieceColor color);

	Board setBoardPiece(Board board, Piece piece, Bitboard bb);

	std::vector<Pos> getBoardColorPositions(Board board, PieceColor color);

	bool isBoardGameOver(Board board, PieceColor *winner);
}