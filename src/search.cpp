#include "position.h"
#include "search.h"
#include "movegen.h"
#include "thread.h"
#include "tt.h"
#include "cli.h"

#include <iostream>

using namespace Search;

namespace Search {

TimePoint StartTime;

} // namespace Search

namespace {

Value negamax(Position &pos, Depth depth, Value alpha, Value beta, int color);

} // namespace

void Search::clear() {

	Threads.main()->wait_for_search_finished();
	
	TT.clear();

	for (Thread *th : Threads)
		th->clear();

	Threads.main()->prevScore = -VALUE_INFINITE;
	Threads.bestThread = Threads.main();
}

void MainThread::search() {

	TT.new_search();

	for (Thread *th : Threads)
		if (th != this)
			th->start_searching();
	Thread::search();

	while (!Threads.stop) {}

	for (Thread *th : Threads)
		if (th != this)
			th->wait_for_search_finished();

	Thread *bestThread = this;
	if (rootMoves[0].pv != MOVE_NONE) {
		for (Thread *th : Threads) {
			Depth depthDiff = th->completedDepth - bestThread->completedDepth;
			Value scoreDiff = th->rootMoves[0].score - bestThread->rootMoves[0].score;
			if (scoreDiff > 0 && (depthDiff >= 0 || th->rootMoves[0].score >= VALUE_WIN))
				bestThread = th;
		}
	}

	prevScore = bestThread->rootMoves[0].score;
	Threads.bestThread = bestThread;
}

void Thread::search() {
	MainThread *mainThread = (this == Threads.main() ? Threads.main() : nullptr);

	Depth rootDepth = DEPTH_ZERO;

	while ((rootDepth += ONE_PLY) < MAX_PLY && !Threads.stop) {
		if (id && rootDepth / ONE_PLY < id)
			continue;

		StateInfo st;
		for (RootMove &rootMove : rootMoves) {

			rootPos.do_move(rootMove.pv, st);

			rootMove.score = -negamax(rootPos, rootDepth, -VALUE_INFINITE, VALUE_INFINITE, 1);

			rootPos.undo_move(rootMove.pv);
		}

		std::stable_sort(rootMoves.begin(), rootMoves.end());

		if (mainThread && CLI::Debug)
			CLI::printPV(rootPos, rootDepth);

		if(rootMoves[0].score >= VALUE_WIN)
			Threads.stop = true;

		if (Threads.stop)
			completedDepth = rootDepth;
	}
}

namespace {

Value negamax(Position &pos, Depth depth, Value alpha, Value beta, int color) {

	StateInfo st;
	Move move;
	Value value, bestValue;
	bestValue = -VALUE_INFINITE;

	Thread *thisThread = pos.this_thread();
	thisThread->nodes++;

	if (thisThread == Threads.main() && max_time(StartTime))
		Threads.stop = true;

	Color win;
	if (pos.is_win(win))
		return color * (win == RED ? -VALUE_WIN : VALUE_WIN);

	if (depth <= DEPTH_ZERO)
		return color * pos.score();

	bool ttHit;
	TTEntry *tte = TT.probe(pos.key(), ttHit);
	Value ttValue = (ttHit ? tte->value() : VALUE_NONE);
	Depth ttDepth = (ttHit ? tte->depth() : DEPTH_NONE);
	Move ttMove = (ttHit ? tte->move() : MOVE_NONE);
	Bound ttBound = (ttHit ? tte->bound() : BOUND_NONE);

	Value alphaOrig = alpha;
	if (ttHit && ttDepth >= depth && ttValue != VALUE_NONE) {
		if (ttBound == BOUND_EXACT)
			return ttValue;
		else if (ttBound == BOUND_LOWER)
			alpha = std::max(alpha, ttValue);
		else if (ttBound == BOUND_UPPER)
			beta = std::min(beta, ttValue);
		if (alpha >= beta)
			return ttValue;
	}

	MovePicker mp(pos, ttMove);
	while ((move = mp.next_move()) != MOVE_NONE) {
		pos.do_move(move, st);
		value = -negamax(pos, depth - ONE_PLY, -beta, -alpha, -color);
		pos.undo_move(move);

		if (Threads.stop.load(std::memory_order_relaxed))
			return VALUE_ZERO;

		bestValue = std::max(bestValue, value);
		alpha = std::max(alpha, value);
		if (alpha >= beta)
			break;
	}

	tte->save(pos.key(), bestValue, (bestValue <= alphaOrig ? BOUND_UPPER : (bestValue >= beta ? BOUND_LOWER : BOUND_EXACT)), depth, MOVE_NONE, VALUE_NONE, TT.generation());

	return bestValue;
}

} // namespace


void CLI::printPV(Position &pos, Depth depth) {
	int elapsed = int(now() - StartTime) + 1;
	const RootMoves &rootMoves = pos.this_thread()->rootMoves;
	uint64_t nodesSearched = Threads.nodes_searched();

	std::printf("depth %2d | score %6d | nodes %10I64d | nps %10I64d | time %6dms | pv %s\n", 
		depth, 
		rootMoves[0].score, 
		nodesSearched, 
		nodesSearched * 1000 / elapsed,
		elapsed, 
		CLI::encode_move(rootMoves[0].pv).c_str());
}