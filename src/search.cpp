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

enum NodeType {
	PV, NonPV
};

template<NodeType NT>
Value negamax(Position &pos, Depth depth, Value alpha, Value beta, Stack *ss);

} // namespace

/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Heavily modified code snippet from Stockfish <search.cpp>
<modified_code>
*/
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
/*
</modified_code>
*/

/*
Inspired by Stockfish thread search
Inspired by https://chessprogramming.wikispaces.com/Lazy+SMP
*/
void Thread::search() {
	MainThread *mainThread = (this == Threads.main() ? Threads.main() : nullptr);

	Stack stack[MAX_PLY + 7], *ss = stack + 4;
	std::memset(ss - 4, 0, 7 * sizeof(Stack));

	Depth rootDepth = DEPTH_ZERO;
	Value value;
	int moveCount;

	while ((rootDepth += ONE_PLY) < MAX_PLY && !Threads.stop) {
		if (id && rootDepth / ONE_PLY < id)
			continue;

		moveCount = 0;
		StateInfo st;
		for (RootMove &rootMove : rootMoves) {

			moveCount++;

			rootPos.do_move(rootMove.pv, st);

			ss->currentMove = rootMove.pv;
			if (moveCount == 1) {
				ss->pv = rootMove.pv;
				value = -negamax<PV>(rootPos, rootDepth, -VALUE_INFINITE, VALUE_INFINITE, ss);
			}
			else {
				value = -negamax<NonPV>(rootPos, rootDepth, -VALUE_INFINITE, VALUE_INFINITE, ss);
			}

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

		if (rootMoves[0].score >= VALUE_WIN)
			Threads.stop = true;

		if (Threads.stop)
			completedDepth = rootDepth;
	}
}

namespace {

/*
Inspired by Stockfish
Inspired by https://en.wikipedia.org/wiki/Negamax
*/
template<NodeType NT>
Value negamax(Position &pos, Depth depth, Value alpha, Value beta, Stack *ss) {

	const bool PvNode = (NT == PV);

	StateInfo st;
	Move move;
	Value value, bestValue;
	bestValue = -VALUE_INFINITE;

	Thread *thisThread = pos.this_thread();
	thisThread->nodes++;

	ss->currentMove = (ss->currentMove < MOVE_NONE ? MOVE_NONE : ss->currentMove);
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

	/*
	Modified code snippet from Stockfish <search.cpp>
	<modified_code>
	*/
	bool ttHit;
	TTEntry *tte = TT.probe(pos.key(), ttHit);
	Value ttValue = (ttHit ? tte->value() : VALUE_NONE);
	Depth ttDepth = (ttHit ? tte->depth() : DEPTH_NONE);
	Move ttMove = (ttHit ? tte->move() : MOVE_NONE);
	Bound ttBound = (ttHit ? tte->bound() : BOUND_NONE);
	/*
	</modified_code>
	*/

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
	
	int moveCount = 0;
	MovePicker mp(pos, ss->ply, ss->currentMove, ttMove, ss->killers, thisThread);
	while ((move = mp.next_move()) != MOVE_NONE) {
		pos.do_move(move, st);
		(ss + 1)->currentMove = move;
		moveCount++;

		if (PvNode && moveCount == 1) {
			value = -negamax<PV>(pos, depth - ONE_PLY, -beta, -alpha, ss + 1);
		}
		else {
			value = -negamax<NonPV>(pos, depth - ONE_PLY, -beta, -alpha, ss + 1);
		}
		
		pos.undo_move(move);

		if (moveCount < THREAD_MOVECOUNT_NB) {
			thisThread->moveCount[moveCount]++;
		}

		if (Threads.stop.load(std::memory_order_relaxed))
			return VALUE_ZERO;

		if (value > bestValue)
			ttMove = move;

		thisThread->history[pos.side_to_move()][move][ss->ply] = value;
		
		bestValue = std::max(bestValue, value);
		alpha = std::max(alpha, value);
		if (alpha >= beta) {

			thisThread->counterMoves[ss->currentMove] = move;

			if (ss->killers[0] != move) {
				ss->killers[1] = ss->killers[0];
				ss->killers[0] = move;
			}

			break;
		}
	}

	if (PvNode) {
		ss->pv = ttMove;
	}

	/*
	Modified code snippet from Stockfish <search.cpp>
	<modified_code>
	*/
	tte->save(pos.key(), bestValue, (bestValue <= alphaOrig ? BOUND_UPPER : (bestValue >= beta ? BOUND_LOWER : BOUND_EXACT)), depth, ttMove, VALUE_NONE, TT.generation());
	/*
	</modified_code>
	*/

	thisThread->ttSaves++;

	return bestValue;
}

} // namespace

/*
Inspired by Stockfish's debug output
*/
void CLI::printPV(Position &pos, Depth depth) {
	int elapsed = int(now() - StartTime) + 1;
	const RootMoves &rootMoves = pos.this_thread()->rootMoves;
	uint64_t nodesSearched = Threads.nodes_searched();

	uint64_t ttHits = 0;
	uint64_t ttSaves = 0;
	float movePercent[THREAD_MOVECOUNT_NB];
	std::memset(movePercent, 0, THREAD_MOVECOUNT_NB * sizeof(float));

	for (Thread *thread : Threads) {
		ttHits += thread->ttHits;
		ttSaves += thread->ttSaves;
		for (int i = 1; i < THREAD_MOVECOUNT_NB; i++) {
			movePercent[i] += (thread->moveCount[i] / (float)nodesSearched) * 100.0f;
		}
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

	std::cout << "                          distr ";
	for (int i = 1; i < THREAD_MOVECOUNT_NB; i++) {
		std::printf("%2.2f%% ", movePercent[i]);
	}
	std::cout << std::endl;
}