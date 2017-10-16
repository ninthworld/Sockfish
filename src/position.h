#pragma once

#include "bitboard.h"
#include "types.h"

struct StateInfo {
};

class Position {
public:
	static void init();

	Position();

	void set_starting(Color moveFirst);

	Bitboard pieces() const;
	Bitboard pieces(PieceType pt) const;
	Bitboard pieces(Color c) const;
	Bitboard pieces(Color c, PieceType pt) const;
	Piece piece_on(Square s) const;
	const Square* squares(Color c, PieceType pt) const;

	Bitboard moves(Square s) const;

	Color side_to_move() const;
	int game_ply() const;

private:
	void put_piece(Piece pc, Square sq);
	void remove_piece(Piece pc, Square s);
	void move_piece(Piece pc, Square from, Square to);

	Piece board[SQUARE_NB];
	Bitboard byTypeBB[PIECE_TYPE_NB];
	Bitboard byColorBB[COLOR_NB];
	int pieceCount[PIECE_NB];
	Square pieceList[PIECE_NB][8];
	int index[SQUARE_NB];

	int gamePly;
	Color sideToMove;

}; // class Position

inline Bitboard Position::pieces() const {
	return byTypeBB[ALL_PIECES];
}

inline Bitboard Position::pieces(PieceType pt) const {
	return byTypeBB[pt];
}

inline Bitboard Position::pieces(Color c) const {
	return byColorBB[c];
}

inline Bitboard Position::pieces(Color c, PieceType pt) const {
	return byColorBB[c] & byTypeBB[pt];
}

inline Piece Position::piece_on(Square s) const {
	return board[s];
}

inline const Square* Position::squares(Color c, PieceType pt) const {
	return pieceList[make_piece(c, pt)];
}

inline Bitboard Position::moves(Square s) const {
	return (moves_bb(s, board[s], byTypeBB[ALL_PIECES]) |
			attacks_bb(s, board[s], byColorBB[WHITE], byColorBB[RED]));
}

inline Color Position::side_to_move() const {
	return sideToMove;
}

inline int Position::game_ply() const {
	return gamePly;
}

inline void Position::put_piece(Piece pc, Square s) {
	board[s] = pc;
	byTypeBB[ALL_PIECES] |= s;
	byTypeBB[type_of(pc)] |= s;
	byColorBB[color_of(pc)] |= s;
	index[s] = pieceCount[pc]++;
	pieceList[pc][index[s]] = s;
	pieceCount[make_piece(color_of(pc), ALL_PIECES)]++;
}

inline void Position::remove_piece(Piece pc, Square s) {
	board[s] = NO_PIECE;
	byTypeBB[ALL_PIECES] ^= s;
	byTypeBB[type_of(pc)] ^= s;
	byColorBB[color_of(pc)] ^= s;

	Square lastSquare = pieceList[pc][--pieceCount[pc]];
	index[lastSquare] = index[s];
	pieceList[pc][index[lastSquare]] = lastSquare;
	pieceList[pc][pieceCount[pc]] = SQ_NONE;
	pieceCount[make_piece(color_of(pc), ALL_PIECES)]--;
}

inline void Position::move_piece(Piece pc, Square from, Square to) {
	Bitboard from_to_bb = SquareBB[from] ^ SquareBB[to];
	byTypeBB[ALL_PIECES] ^= from_to_bb;
	byTypeBB[type_of(pc)] ^= from_to_bb;
	byColorBB[color_of(pc)] ^= from_to_bb;
	board[from] = NO_PIECE;
	board[to] = pc;
	index[to] = index[from];
	pieceList[pc][index[to]] = to;
}