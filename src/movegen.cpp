#include "movegen.h"
#include "position.h"

namespace {

} // namespace

ExtMove* generate(const Position &pos, ExtMove *moveList) {
	Color c = pos.side_to_move();

	for (PieceType pt = MINI_NINJA; pt <= SAMURAI; ++pt) {
		const Square *pl = pos.squares(c, pt);

		for (Square from = *pl; from != SQ_NONE; from = *++pl) {
			Bitboard b = pos.moves(from);

			while (b)
				*moveList++ = make_move(from, pop_lsb(&b));
		}
	}

	return moveList;
}