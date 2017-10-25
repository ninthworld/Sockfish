#include "movegen.h"
#include "position.h"
#include "search.h"
#include "thread.h"

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

MovePicker::MovePicker(const Position &p, const Move currentMove, const Move ttMove, const Move killers[], const Move counterMove, Thread *thread)
	: pos(p)
	, cur(moves)
	, endMoves(generate(pos, cur)) {
	
	ExtMove *c = cur;
	do {
		c->score = thread->history[p.side_to_move()][c->move];
		c->score = std::min(c->score, VALUE_INFINITE - 20);
	} while (++c < endMoves);
	
	/*

	if (Search::SearchData != nullptr) {
		ExtMove *c = cur;
		do {
			c->score = Search::SearchData->history[pos.side_to_move()][c->move];
			if (Search::SearchData->countermove[currentMove] == c->move)
				c->score = Value(10000);
		} while (++c < endMoves);
	}*/

	if (ttMove != MOVE_NONE) {
		ExtMove *m;
		if ((m = std::find(cur, endMoves, ttMove))) {
			m->score = VALUE_INFINITE - 1;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (killers[i] != MOVE_NONE) {
			ExtMove *m;
			if ((m = std::find(cur, endMoves, killers[i]))) {
				m->score = VALUE_INFINITE - i - 2;
			}
		}
	}

	if (counterMove != MOVE_NONE) {
		ExtMove *m;
		if ((m = std::find(cur, endMoves, counterMove))) {
			m->score = VALUE_INFINITE - 10;
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