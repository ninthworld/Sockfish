#include "bitboard.h"
#include "position.h"
#include "thread.h"

void Position::init() {
	// Zobrist Key
}

Position::Position() {

}

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
	
	/*
	// TEST CASE
	put_piece(W_MINI_SAMURAI, SQ_E3);	put_piece(W_MINI_SAMURAI, SQ_F3);	put_piece(W_MINI_SAMURAI, SQ_G3);
	put_piece(W_MINI_NINJA, SQ_B4);		put_piece(W_MINI_NINJA, SQ_C5);
	put_piece(W_NINJA, SQ_D5);			put_piece(W_NINJA, SQ_F2);			put_piece(W_NINJA, SQ_G2);
	put_piece(W_SAMURAI, SQ_A4);		put_piece(W_SAMURAI, SQ_B2);		put_piece(W_SAMURAI, SQ_C2);
	put_piece(W_KING, SQ_D1);

	put_piece(R_MINI_NINJA, SQ_A5);		put_piece(R_MINI_NINJA, SQ_C6);		put_piece(R_MINI_NINJA, SQ_D6);
	put_piece(R_MINI_NINJA, SQ_E6);		put_piece(R_MINI_NINJA, SQ_F6);		put_piece(R_MINI_NINJA, SQ_G6);
	put_piece(R_SAMURAI, SQ_E7);		put_piece(R_SAMURAI, SQ_F7);		put_piece(R_SAMURAI, SQ_G7);
	put_piece(R_KING, SQ_D8);
	*/

	sideToMove	= moveFirst;
	gamePly		= 0;
	sideAi = ai;

	st = si;
	thisThread = th;
}

void Position::set(Position &pos, StateInfo *si, Thread *th) {
	std::memcpy(&(th->rootPos), &pos, sizeof(Position));
	st = si;
	thisThread = th;

}

void Position::do_move(Move m, StateInfo &newSt) {
	
	newSt.previous = st;
	st = &newSt;

	++gamePly;

	Color c = sideToMove;
	Color nC = ~c;
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = piece_on(from);

	move_piece(pc, from, to);
	
	Rank attackRank = rank_of(to) + (Rank)(c == WHITE ? 1 : -1);
	if (attackRank > Rank8BB)
		attackRank = rank_of(to);

	Square attackSquare = make_square(file_of(to), attackRank);
	Piece attackPc = piece_on(attackSquare);
	if (attackPc != NO_PIECE) {
		if (color_of(attackPc) == nC) {
			remove_piece(attackPc, attackSquare);

			PieceType attackPt = type_of(attackPc);
			if (attackPt == NINJA) {
				put_piece(make_piece(nC, MINI_NINJA), attackSquare);
			}
			else if (attackPt == SAMURAI) {
				put_piece(make_piece(nC, MINI_SAMURAI), attackSquare);
			}
		}
		else {
			attackPc = NO_PIECE;
		}
	}

	newSt.attackedPiece = attackPc;

	sideToMove = nC;
}

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