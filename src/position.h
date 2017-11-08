#pragma once

#include <deque>
#include <memory>

#include "bitboard.h"
#include "types.h"

/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Modified code snippet from Stockfish <position.h>
<modified_code>
*/
struct StateInfo {
	Key key;
	Piece attackedPiece;
	StateInfo *previous;
};

typedef std::unique_ptr<std::deque<StateInfo>> StateListPtr;

class Thread;

class Position {
public:
	static void init();

	void set_starting(Color moveFirst, Color ai, StateInfo *si, Thread *th);
	void set(Position &pos, StateInfo *si, Thread *th);

	Bitboard pieces() const;
	Bitboard pieces(PieceType pt) const;
	Bitboard pieces(Color c) const;
	Bitboard pieces(Color c, PieceType pt) const;
	Piece piece_on(Square s) const;
	const Square* squares(Color c, PieceType pt) const;

	Bitboard moves(Square s) const;

	bool legal(Move m) const;
	bool is_win(Color &winner) const;
	Value score() const;
	Key key() const;

	void do_move(Move m, StateInfo &newSt);
	void undo_move(Move m);

	void do_null_move(StateInfo &newSt);
	void undo_null_move();

	Color side_to_move() const;
	int game_ply() const;

	Thread* this_thread() const;

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
	StateInfo *st;
	Thread* thisThread;

}; // class Position
/*
</modified_code>
*/

/*
Code snippet from Stockfish <position.h>
<code>
*/
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
/*
</code>
*/

/*
Inspired by Stockfish
*/
inline Bitboard Position::moves(Square s) const {
	return (moves_bb(s, board[s], byTypeBB[ALL_PIECES]) |
			attacks_bb(s, board[s], byColorBB[WHITE], byColorBB[RED]));
}

inline bool Position::legal(Move m) const {
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece p = piece_on(from);
	return p != NO_PIECE && (sideToMove == color_of(p)) && (moves(from) & square_bb(to));
}

inline bool Position::is_win(Color &winner) const {
	if (pieceCount[W_KING] < 1) {
		winner = RED;
		return true;
	}

	if (pieceCount[R_KING] < 1) {
		winner = WHITE;
		return true;
	}

	if (sideToMove == WHITE) {
		for (PieceType pt = MINI_NINJA; pt <= SAMURAI; ++pt) {
			const Square *pl = squares(WHITE, pt);
			for (Square from = *pl; from != SQ_NONE; from = *++pl) {
				if (moves(from)) return false;
			}
		}
	}
	else {
		for (PieceType pt = MINI_NINJA; pt <= SAMURAI; ++pt) {
			const Square *pl = squares(RED, pt);
			for (Square from = *pl; from != SQ_NONE; from = *++pl) {
				if (moves(from)) return false;
			}
		}
	}

	winner = ~sideToMove;
	return true;
}

inline Value Position::score() const {
	Value score = Value(0);

	score +=
		(pieceCount[W_MINI_NINJA] - pieceCount[R_MINI_NINJA]) * MiniNinjaValue +
		(pieceCount[W_MINI_SAMURAI] - pieceCount[R_MINI_SAMURAI]) * MiniSamuraiValue +
		(pieceCount[W_NINJA] - pieceCount[R_NINJA]) * NinjaValue +
		(pieceCount[W_SAMURAI] - pieceCount[R_SAMURAI]) * SamuraiValue;

	int moveCount = 0;
	for(Color c = WHITE; c <= RED; ++c){
		for (PieceType pt = MINI_NINJA; pt < KING; ++pt) {
			const Square *pl = squares(c, pt);
			for (Square from = *pl; from != SQ_NONE; from = *++pl) {
				score += ValueMap::Values[make_piece(c, pt)][from];

				if (c == WHITE) {
					moveCount += popcount(moves(from));
				}
				else {
					moveCount -= popcount(moves(from));
				}
			}
		}
	}

	score += moveCount * MoveCountValue;

	return score;
}

/*
Code snippet from Stockfish <position.h>
<code>
*/
inline Key Position::key() const {
	return st->key;
}

inline Color Position::side_to_move() const {
	return sideToMove;
}

inline int Position::game_ply() const {
	return gamePly;
}

inline Thread* Position::this_thread() const {
	return thisThread;
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
/*
</code>
*/