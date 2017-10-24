#include "movegen.h"
#include "position.h"
#include "search.h"

#include <iostream>

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

MovePicker::MovePicker(const Position &p, const Move currentMove, const Move ttMove, const Move killers[])
	: pos(p)
	, cur(moves)
	, endMoves(generate(pos, cur)) {

	if (Search::SearchData != nullptr) {
		ExtMove *c = cur;
		do {
			c->score = Search::SearchData->history[p.side_to_move()][c->move];
			if (Search::SearchData->countermove[currentMove] == c->move)
				c->score = Value(10000);
		} while (++c < endMoves);
	}

	if (ttMove != MOVE_NONE) {
		ExtMove *m;
		if ((m = std::find(cur, endMoves, ttMove))) {
			m->score = VALUE_INFINITE;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (killers[i] != MOVE_NONE) {
			ExtMove *m;
			if ((m = std::find(cur, endMoves, killers[i]))) {
				m->score = VALUE_INFINITE - i - 1;
			}
		}
	}

	std::stable_sort(cur, endMoves);
}

Move MovePicker::next_move() {
	if (cur < endMoves) {
		return (cur++)->move;
	}
	else {
		return MOVE_NONE;
	}
}