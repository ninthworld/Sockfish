#include "bitboard.h"
#include "position.h"

void Position::init() {
	// Zobrist Key
}

Position::Position() {

}

void Position::set_starting(Color moveFirst) {
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
}

//void Position::do_move(Move m, StateInfo &newState, bool givesCheck) {
//
//}