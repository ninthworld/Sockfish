#include "movegen.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "timer.h"
#include "cli.h"

#include <iostream>

namespace Search {

TimePoint StartTime;

} // namespace Search

namespace {

Value search(Position &pos, Value alpha, Value beta, Depth depth, Depth maxDepth, int &nodeCount, int &leafCount);

} // namespace

void Search::clear() {

	Threads.main()->wait_for_search_finished();

	// Time.availableNodes = 0;
	// TT.clear();

	for (Thread *th : Threads)
		th->clear();

	Threads.main()->prevScore = VALUE_INFINITE;
	Threads.bestThread = Threads.main();
}

void MainThread::search() {

	Time.init(Search::StartTime);

	for (Thread *th : Threads)
		if (th != this)
			th->start_searching();
	Thread::search();

	while(!Threads.stop){}

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
	Value bestValue, alpha, beta;
	MainThread *mainThread = (this == Threads.main() ? Threads.main() : nullptr);
	
	bestValue = alpha = -VALUE_INFINITE;
	beta = VALUE_INFINITE;

	// Iterative deepening
	while ((rootDepth += ONE_PLY) < DEPTH_MAX && !Threads.stop) {

		if (id && rootDepth / ONE_PLY < id)
			continue;
		
		int nodeCount = 0, leafCount = 0;
		bestValue = ::search(rootPos, alpha, beta, rootDepth, rootDepth, nodeCount, leafCount);

		if (mainThread) {
			std::cout << "rootDepth=" << rootDepth << ", nodes=" << nodeCount << ", leaves=" << leafCount << ", bestValue=" << bestValue << std::endl;
			if (bestValue == VALUE_WIN) {
				std::cout << "Win in " << int((rootDepth-1)/2) << " moves" << std::endl;
			}
		}

		std::stable_sort(rootMoves.begin(), rootMoves.end());
		
		if (Threads.stop)
			completedDepth = rootDepth;

		if (bestValue >= VALUE_WIN)
			Threads.stop = true;
	}
}

namespace {

Value search(Position &pos, Value alpha, Value beta, Depth depth, Depth maxDepth, int &nodeCount, int &leafCount) {

	nodeCount++;

	bool isRoot = (depth == maxDepth);

	StateInfo st;
	Move move;
	Value value, bestValue;
	
	Thread *thisThread = pos.this_thread();

	bool isMaximizing = (pos.side_to_move() == pos.side_ai());
	bestValue = (isMaximizing ? -VALUE_INFINITE : VALUE_INFINITE);

	Color winColor;
	if (pos.is_win(winColor)) {
		leafCount++;
		return (winColor == pos.side_ai() ? VALUE_WIN : VALUE_LOSE);
	}

	if (depth <= 0) {
		leafCount++;
		return pos.score();
	}

	MovePicker mp(pos);

	while ((move = mp.next_move()) != MOVE_NONE) {
		if (!isRoot && Time.elapsed() > Time.maximum()) {
			Threads.stop = true;
			return bestValue;
		}
		
		pos.do_move(move, st);
		value = ::search(pos, alpha, beta, depth - ONE_PLY, maxDepth, nodeCount, leafCount);
		pos.undo_move(move);
		
		if (isRoot) {
			Search::RootMove &rm = *std::find(thisThread->rootMoves.begin(), thisThread->rootMoves.end(), move);

			rm.score = value;
			rm.pv = move;

			bestValue = std::max(bestValue, value);
		}
		else {
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
	}

	return bestValue;
}

} // namespace

/*
Move Search::best_move_1(Position &rootPos, TimePoint &start, Depth *maxDepth) {

	Depth rootDepth = DEPTH_ZERO;
	Value bestScore = -VALUE_INFINITE, score;
	Move bestMove = MOVE_NONE, move;

	StateInfo st;

	// Iterative deepening loop
	while ((rootDepth += ONE_PLY) < MAX_PLY) {
		MovePicker mp(rootPos);

		while((move = mp.next_move()) != MOVE_NONE){
			if (max_time(start)) {
				if (bestMove == MOVE_NONE) {
					bestMove = move;
				}

				goto exitLoop;
			}

			rootPos.do_move(move, st);
			score = ::search_1(rootPos, -VALUE_INFINITE, VALUE_INFINITE, rootDepth, start);
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
*/

/*
namespace {

Value search_1(Position &pos, Value alpha, Value beta, Depth depth, TimePoint &start) {
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
		if (max_time(start)) {
			return bestValue;
		}

		pos.do_move(move, st);
		value = ::search_1(pos, -VALUE_INFINITE, VALUE_INFINITE, depth - ONE_PLY, start);
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
*/