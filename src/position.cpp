#include "bitboard.h"
#include "position.h"
#include "thread.h"

/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Modified code snippet from Stockfish <position.cpp>
<modified_code>
*/
namespace Zobrist {

Key psq[PIECE_NB][SQUARE_NB];
Key side;

} // namespace Zobrist

void Position::init() {
	
	PRNG rng(1070372);

	for (Piece pc : {NO_PIECE, W_MINI_NINJA, W_MINI_SAMURAI, W_NINJA, W_SAMURAI, W_KING, R_MINI_NINJA, R_MINI_SAMURAI, R_NINJA, R_SAMURAI, R_KING}) {
		for (Square s = SQ_A1; s <= SQ_G8; ++s) {
			Zobrist::psq[pc][s] = rng.rand<Key>();
		}
	}

	Zobrist::side = rng.rand<Key>();
}
/*
</modified_code>
*/

void Position::set_starting(Color moveFirst, Color ai, StateInfo *si, Thread *th) {
	std::memset(this, 0, sizeof(Position));

	for(Piece p = W_MINI_NINJA; p <= R_KING; ++p)
		for (int i = 0; i < 8; ++i)
			pieceList[p][i] = SQ_NONE;
	
	put_piece(W_MINI_NINJA, SQ_A3);		put_piece(W_MINI_NINJA, SQ_B3);		put_piece(W_MINI_NINJA, SQ_C3);
	put_piece(W_MINI_SAMURAI, SQ_E3);	put_piece(W_MINI_SAMURAI, SQ_F3);	put_piece(W_MINI_SAMURAI, SQ_G3);
	put_piece(W_SAMURAI, SQ_A2);		put_piece(W_SAMURAI, SQ_B2);		put_piece(W_SAMURAI, SQ_C2);
	put_piece(W_NINJA, SQ_E2);			put_piece(W_NINJA, SQ_F2);			put_piece(W_NINJA, SQ_G2);
	put_piece(W_KING, SQ_D1);

	put_piece(R_MINI_SAMURAI, SQ_A6);	put_piece(R_MINI_SAMURAI, SQ_B6);	put_piece(R_MINI_SAMURAI, SQ_C6);
	put_piece(R_MINI_NINJA, SQ_E6);		put_piece(R_MINI_NINJA, SQ_F6);		put_piece(R_MINI_NINJA, SQ_G6);
	put_piece(R_NINJA, SQ_A7);			put_piece(R_NINJA, SQ_B7);			put_piece(R_NINJA, SQ_C7);
	put_piece(R_SAMURAI, SQ_E7);		put_piece(R_SAMURAI, SQ_F7);		put_piece(R_SAMURAI, SQ_G7);
	put_piece(R_KING, SQ_D8);

	sideToMove	= moveFirst;
	gamePly		= 0;

	st = si;
	thisThread = th;
}

void Position::set(Position &pos, StateInfo *si, Thread *th) {

	std::memcpy(&(th->rootPos), &pos, sizeof(Position));
	st = si;
	thisThread = th;
}

/*
Inspired by Stockfish's move operation
*/
void Position::do_move(Move m, StateInfo &newSt) {
	
	Key k = st->key ^ Zobrist::side;

	newSt.previous = st;
	st = &newSt;

	++gamePly;

	Color c = sideToMove;
	Color nC = ~c;
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = piece_on(from);

	move_piece(pc, from, to);

	k ^= Zobrist::psq[pc][from] ^ Zobrist::psq[pc][to];
	
	Rank attackRank = rank_of(to) + (Rank)(c == WHITE ? 1 : -1);
	if (attackRank > Rank8BB)
		attackRank = rank_of(to);

	Square attackSquare = make_square(file_of(to), attackRank);
	Piece attackPc = piece_on(attackSquare);
	if (attackPc != NO_PIECE) {
		if (color_of(attackPc) == nC) {
			remove_piece(attackPc, attackSquare);

			k ^= Zobrist::psq[attackPc][attackSquare] ^ Zobrist::psq[NO_PIECE][attackSquare];

			PieceType attackPt = type_of(attackPc);
			if (attackPt == NINJA) {
				Piece newPiece = make_piece(nC, MINI_NINJA);
				put_piece(newPiece, attackSquare);

				k ^= Zobrist::psq[attackPc][attackSquare] ^ Zobrist::psq[newPiece][attackSquare];
			}
			else if (attackPt == SAMURAI) {
				Piece newPiece = make_piece(nC, MINI_SAMURAI);
				put_piece(newPiece, attackSquare);

				k ^= Zobrist::psq[attackPc][attackSquare] ^ Zobrist::psq[newPiece][attackSquare];
			}
		}
		else {
			attackPc = NO_PIECE;
		}
	}

	st->attackedPiece = attackPc;

	sideToMove = nC;

	st->key = k;
}

/*
Inspired by Stockfish's undo move operation
*/
void Position::undo_move(Move m) {

	sideToMove = ~sideToMove;

	Color c = sideToMove;
	Color nC = ~c;
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = piece_on(to);

	move_piece(pc, to, from);

	if (st->attackedPiece != NO_PIECE) {
		Square attackedSquare = make_square(file_of(to), rank_of(to) +(Rank)(c == WHITE ? 1 : -1));

		PieceType attackPt = type_of(st->attackedPiece);
		if (attackPt == NINJA) {
			remove_piece(make_piece(nC, MINI_NINJA), attackedSquare);
		}
		else if (attackPt == SAMURAI) {
			remove_piece(make_piece(nC, MINI_SAMURAI), attackedSquare);
		}

		put_piece(st->attackedPiece, attackedSquare);
	}
	
	st = st->previous;
	--gamePly;
}

void Position::do_null_move(StateInfo &newSt) {

	Key k = st->key ^ Zobrist::side;

	newSt.previous = st;
	st = &newSt;

	++gamePly;

	Color c = sideToMove;
	Color nC = ~c;

	sideToMove = nC;

	st->key = k;
}

void Position::undo_null_move() {

	sideToMove = ~sideToMove;

	st = st->previous;
	--gamePly;
}