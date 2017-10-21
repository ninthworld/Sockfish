#include "movegen.h"
#include "position.h"

#include "cli.h"

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

MovePicker::MovePicker(const Position &p, const Move pv)
	: pos(p)
	, cur(moves)
	, endMoves(generate(pos, cur)) {

	if (pv != MOVE_NONE) {
		ExtMove *m;
		if ((m = std::find(cur, endMoves, pv))) {
			m->score = VALUE_INFINITE;
			std::stable_sort(cur, endMoves);
		}
	}
}

Move MovePicker::next_move() {
	if (cur < endMoves) {
		return (cur++)->move;
	}
	else {
		return MOVE_NONE;
	}
}