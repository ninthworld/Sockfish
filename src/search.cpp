#include "movegen.h"
#include "position.h"
#include "search.h"
#include "cli.h"

#include <iostream>

namespace {

Value search(Position &pos, Value alpha, Value beta, Depth depth, TimePoint &start);

} // namespace

Move Search::best_move(Position &rootPos, TimePoint &start, Depth *maxDepth) {

	Depth rootDepth = DEPTH_ZERO;
	Value bestScore = -VALUE_INFINITE, score;
	Move bestMove = MOVE_NONE, move;

	StateInfo st;

	// Iterative deepening loop
	while ((rootDepth += ONE_PLY) < MAX_PLY) {
		MovePicker mp(rootPos);

		while((move = mp.next_move()) != MOVE_NONE){
			if (now() - start > MAX_TIME) {
				if (bestMove == MOVE_NONE) {
					bestMove = move;
				}

				goto exitLoop;
			}

			rootPos.do_move(move, st);
			score = ::search(rootPos, -VALUE_INFINITE, VALUE_INFINITE, rootDepth, start);
			rootPos.undo_move(move);
			
			if (score > bestScore || bestMove == MOVE_NONE) {
				bestScore = score;
				bestMove = move;

				if (bestScore == VALUE_INFINITE) {
					goto exitLoop;
				}
			}
		}
	}

exitLoop:

	if(maxDepth != nullptr)
		*maxDepth = rootDepth;

	return bestMove;
}

namespace {

Value search(Position &pos, Value alpha, Value beta, Depth depth, TimePoint &start) {
	if (depth == DEPTH_ZERO) {
		return pos.score();
	}

	Color winner;
	if (pos.is_win(winner)) {
		return (pos.side_ai() == winner ? 1 : -1) * VALUE_INFINITE;
	}

	bool isMaximizing = (pos.side_to_move() == pos.side_ai());
	Value bestValue = (isMaximizing ? -VALUE_INFINITE : VALUE_INFINITE), value;

	StateInfo st;
	MovePicker mp(pos);
	Move move;

	while ((move = mp.next_move()) != MOVE_NONE) {
		if (now() - start > MAX_TIME) {
			return bestValue;
		}

		pos.do_move(move, st);
		value = ::search(pos, -VALUE_INFINITE, VALUE_INFINITE, depth - ONE_PLY, start);
		pos.undo_move(move);

		if (isMaximizing) {
			bestValue = std::max(bestValue, value);
			alpha = std::max(alpha, bestValue);
			if (beta <= alpha)
				break;
		}
		else {
			bestValue = std::min(bestValue, value);
			beta = std::min(beta, bestValue);
			if (beta <= alpha)
				break;
		}
	}

	return bestValue;
}

} // namespace