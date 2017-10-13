#include "bitboards.h"

Pos getPos(Bitboard bb) {
	for (int row = 0; row < ROWS; ++row) {
		for (int col = 0; col < COLS; ++col) {
			if (bb & Bitboards::getTileAt({ row, col })) {
				return { row, col };
			}
		}
	}
}

int getPosIndex(Pos pos) {
	return pos.row * COLS + pos.col;
}

Bitboard Bitboards::tileBB[ROWS][COLS];

void Bitboards::init() {
	for (int row = 0; row < ROWS; ++row) {
		for (int col = 0; col < COLS; ++col) {
			tileBB[row][col] = (0x1ULL << getPosIndex({ row, col }));
		}
	}
}

Bitboard Bitboards::getTileAt(Pos pos) {
	return tileBB[pos.row][pos.col];
}

Board Bitboards::createBoard() {
	Board board;

	board.piece[WHITE][MINI_NINJA] = 0x1c000ULL;
	board.piece[WHITE][MINI_SAMURAI] = 0x1c0000ULL;
	board.piece[WHITE][NINJA] = 0x3800ULL;
	board.piece[WHITE][SAMURAI] = 0x380ULL;
	board.piece[WHITE][KING] = 0x8ULL;

	board.piece[RED][MINI_NINJA] = 0x38000000000ULL;
	board.piece[RED][MINI_SAMURAI] = 0x3800000000ULL;
	board.piece[RED][NINJA] = 0x1c0000000000ULL;
	board.piece[RED][SAMURAI] = 0x1c00000000000ULL;
	board.piece[RED][KING] = 0x10000000000000ULL;

	board.pieceCount[WHITE][MINI_NINJA] = 3;
	board.pieceCount[WHITE][MINI_SAMURAI] = 3;
	board.pieceCount[WHITE][NINJA] = 3;
	board.pieceCount[WHITE][SAMURAI] = 3;
	board.pieceCount[WHITE][KING] = 1;

	board.pieceCount[RED][MINI_NINJA] = 3;
	board.pieceCount[RED][MINI_SAMURAI] = 3;
	board.pieceCount[RED][NINJA] = 3;
	board.pieceCount[RED][SAMURAI] = 3;
	board.pieceCount[RED][KING] = 1;

	return board;
}

Piece Bitboards::getPieceAt(Board board, Pos pos) {
	Bitboard selector = getTileAt(pos);

	for (unsigned int i = WHITE; i < PIECE_COLOR_NB; ++i) {
		for (unsigned int j = MINI_NINJA; j < PIECE_TYPE_NB; ++j) {
			if (board.piece[i][j] & selector) {
				return { (PieceColor)i, (PieceType)j };
			}
		}
	}

	return { NO_PIECE_COLOR, NO_PIECE_TYPE };
}

Bitboard Bitboards::getBoardPiece(Board board, Piece piece) {
	return board.piece[piece.color][piece.type];
}

Bitboard Bitboards::getBoardColor(Board board, PieceColor color) {
	Bitboard bb = 0x0ULL;

	for (unsigned int i = MINI_NINJA; i < PIECE_TYPE_NB; ++i) {
		bb |= getBoardPiece(board, { color, (PieceType)i });
	}

	return bb;
}

Board Bitboards::setBoardPiece(Board board, Piece piece, Bitboard bb) {
	board.piece[piece.color][piece.type] = bb;
	return board;
}

std::vector<Pos> Bitboards::getBoardColorPositions(Board board, PieceColor color) {
	Bitboard colorBB = getBoardColor(board, color) ^ getBoardPiece(board, { color, KING });
	std::vector<Pos> positions;

	int index = 0;
	do {
		if (colorBB & 0x1ULL) {
			positions.push_back({ (int)floor(index/COLS), index%COLS });
		}
		index++;
	} while (colorBB >>= 1);

	return positions;
}

bool Bitboards::isBoardGameOver(Board board, PieceColor *winner) {
	if (winner != nullptr) {
		if (!board.pieceCount[WHITE][KING]) {
			*winner = RED;
		}
		else if (!board.pieceCount[RED][KING]) {
			*winner = WHITE;
		}
		else {
			*winner = NO_PIECE_COLOR;
		}
	}

	return (!board.pieceCount[WHITE][KING] || !board.pieceCount[RED][KING]);
}
