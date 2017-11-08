#include "movegen.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "cli.h"

/*
Heavily modified from Stockfish <movegen.cpp>
<modified_code>
*/
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
/*
</modified_code>
*/

/*
Inspired by Stockfish's move generation ordering
*/
MovePicker::MovePicker(const Position &p, const int ply, const Move currentMove, const Move ttMove, const Move killers[], Thread *thread)
	: pos(p)
	, cur(moves)
	, endMoves(generate(pos, cur)) {
	
	ExtMove *m = cur;
	do {
		if (m->move == ttMove) {
			m->score = VALUE_INFINITE - 1;
		}
		else if (m->move == killers[0]) {
			m->score = VALUE_INFINITE - 2;
		}
		else if (m->move == killers[1]) {
			m->score = VALUE_INFINITE - 3;
		}
		else if (m->move == thread->counterMoves[currentMove]) {
			m->score = VALUE_INFINITE - 4;
		}
		else {
			m->score = thread->history[p.side_to_move()][m->move][ply];
		}
	} while (++m < endMoves);

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