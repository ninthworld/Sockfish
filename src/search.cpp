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

SearchInfo *SearchData;

} // namespace Search

namespace {

Value negamax(Position &pos, Depth depth, Value alpha, Value beta, Stack *ss);

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

	SearchData = new SearchInfo();
	std::memset(SearchData, 0, sizeof(SearchInfo));

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

	delete SearchData;
}

void Thread::search() {
	MainThread *mainThread = (this == Threads.main() ? Threads.main() : nullptr);

	Stack stack[MAX_PLY + 7], *ss = stack + 4;
	std::memset(ss - 4, 0, 7 * sizeof(Stack));

	Depth rootDepth = DEPTH_ZERO;
	Value value;

	while ((rootDepth += ONE_PLY) < MAX_PLY && !Threads.stop) {
		if (id && rootDepth / ONE_PLY < id)
			continue;

		StateInfo st;
		for (RootMove &rootMove : rootMoves) {

			rootPos.do_move(rootMove.pv, st);

			ss->currentMove = rootMove.pv;
			value = -negamax(rootPos, rootDepth, -VALUE_INFINITE, VALUE_INFINITE, ss);

			rootPos.undo_move(rootMove.pv);

			if (rootDepth > 5 * ONE_PLY && value == VALUE_ZERO) {
				rootMove.score = rootMove.prevScore;
			}
			else {
				rootMove.prevScore = rootMove.score;
				rootMove.score = value;
			}
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

Value negamax(Position &pos, Depth depth, Value alpha, Value beta, Stack *ss) {

	StateInfo st;
	Move move;
	Value value, bestValue;
	bestValue = -VALUE_INFINITE;

	Thread *thisThread = pos.this_thread();
	thisThread->nodes++;

	ss->ply = (ss - 1)->ply + 1;
	(ss + 2)->killers[0] = (ss + 2)->killers[1] = MOVE_NONE;

	int color = (ss->ply % 2 ? 1 : -1);

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
		thisThread->ttHits++;

		if (ttBound == BOUND_EXACT)
			return ttValue;
		else if (ttBound == BOUND_LOWER)
			alpha = std::max(alpha, ttValue);
		else if (ttBound == BOUND_UPPER)
			beta = std::min(beta, ttValue);
		if (alpha >= beta)
			return ttValue;
	}

	MovePicker mp(pos, ss->currentMove, ttMove, ss->killers);
	while ((move = mp.next_move()) != MOVE_NONE) {
		pos.do_move(move, st);
		(ss + 1)->currentMove = move;
		value = -negamax(pos, depth - ONE_PLY, -beta, -alpha, ss + 1);
		pos.undo_move(move);

		if (Threads.stop.load(std::memory_order_relaxed))
			return VALUE_ZERO;

		if (value > bestValue)
			ttMove = move;

		// SearchData->history[pos.side_to_move()][move] = value;

		bestValue = std::max(bestValue, value);
		alpha = std::max(alpha, value);
		if (alpha >= beta) {
			SearchData->countermove[ss->currentMove] = move;
			SearchData->history[pos.side_to_move()][move] = Value(depth * depth);
			break;
		}
	}

	tte->save(pos.key(), bestValue, (bestValue <= alphaOrig ? BOUND_UPPER : (bestValue >= beta ? BOUND_LOWER : BOUND_EXACT)), depth, ttMove, VALUE_NONE, TT.generation());
	thisThread->ttSaves++;

	if (ss->killers[0] != move) {
		ss->killers[1] = ss->killers[0];
		ss->killers[0] = move;
	}

	return bestValue;
}

} // namespace


void CLI::printPV(Position &pos, Depth depth) {
	int elapsed = int(now() - StartTime) + 1;
	const RootMoves &rootMoves = pos.this_thread()->rootMoves;
	uint64_t nodesSearched = Threads.nodes_searched();

	uint64_t ttHits = 0;
	uint64_t ttSaves = 0;

	for (Thread *thread : Threads) {
		ttHits += thread->ttHits;
		ttSaves += thread->ttSaves;
	}

	std::printf("depth %2d | score %6d | nodes %10I64d | nps %10I64d | time %6dms | pv %s | hits %8I64d | saves %8I64d\n", 
		depth, 
		rootMoves[0].score, 
		nodesSearched, 
		nodesSearched * 1000 / elapsed,
		elapsed, 
		CLI::encode_move(rootMoves[0].pv).c_str(),
		ttHits,
		ttSaves
	);
}