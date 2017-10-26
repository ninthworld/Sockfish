#include "movegen.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "cli.h"

#include <iostream>

namespace {

} // namespace

ExtMove* generate(const Position &pos, ExtMove *moveList) {
	Color c = pos.side_to_move();

	//for (PieceType pt = MINI_NINJA; pt <= SAMURAI; ++pt) {
	for (PieceType pt = SAMURAI; pt >= MINI_NINJA; --pt) {
		const Square *pl = pos.squares(c, pt);

		for (Square from = *pl; from != SQ_NONE; from = *++pl) {
			Bitboard b = pos.moves(from);
			
			while (b) 
				*moveList++ = make_move(from, pop_lsb(&b));
		}
	}

	return moveList;
}

MovePicker::MovePicker(const Position &p, const int ply, const Move currentMove, const Move ttMove, const Move killers[], Thread *thread)
	: pos(p)
	, cur(moves)
	, endMoves(generate(pos, cur)) {
	
	ExtMove *m = cur;
	do {
		m->score = thread->history[p.side_to_move()][m->move][ply];
	} while (++m < endMoves);

	if (ttMove != MOVE_NONE && (m = std::find(cur, endMoves, ttMove))) {
		m->score = VALUE_INFINITE - 1;
	}

	if (killers[0] != MOVE_NONE && (m = std::find(cur, endMoves, killers[0]))) {
		m->score = VALUE_INFINITE - 2;
	}

	if (killers[1] != MOVE_NONE && (m = std::find(cur, endMoves, killers[1]))) {
		m->score = VALUE_INFINITE - 3;
	}

	Move counterMove = thread->counterMoves[currentMove];
	if (counterMove != MOVE_NONE && (m = std::find(cur, endMoves, counterMove))) {
		m->score = VALUE_INFINITE - 4;
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