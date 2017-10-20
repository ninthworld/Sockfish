#include "position.h"
#include "search.h"
#include "movegen.h"
#include "thread.h"
#include "timer.h"
#include "cli.h"

#include <iostream>

using namespace Search;

namespace Search {

TimePoint StartTime;

} // namespace Search

namespace {

enum NodeType { PV, NonPV };

template <NodeType PT>
Value search(Position &pos, Stack *ss, Value alpha, Value beta, Depth depth);

} // namespace

uint64_t Search::perft(Position &pos, Depth depth, bool root) {
	StateInfo st;
	uint64_t cnt, nodes = 0;
	const bool leaf = (depth == 2 * ONE_PLY);

	for (const auto &m : MoveList(pos)) {
		if (root && depth <= ONE_PLY)
			cnt = 1, nodes++;
		else {
			pos.do_move(m, st);
			cnt = (leaf ? MoveList(pos).size() : perft(pos, depth - ONE_PLY, false));
			nodes += cnt;
			pos.undo_move(m);
		}
		if (root)
			std::cout << CLI::encode_move(m) << ": " << cnt << std::endl;
	}

	return nodes;
}


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
	Stack stack[MAX_PLY + 7], *ss = stack + 4;
	Value bestValue, alpha, beta;
	MainThread *mainThread = (this == Threads.main() ? Threads.main() : nullptr);

	std::memset(ss - 4, 0, 32 * sizeof(Stack));

	bestValue = alpha = -VALUE_INFINITE;
	beta = VALUE_INFINITE;

	// Iterative deepening
	while ((rootDepth += ONE_PLY) < DEPTH_MAX && !Threads.stop) {
		if (id && rootDepth / ONE_PLY < id)
			continue;
		
		selDepth = 0;

		bestValue = ::search<PV>(rootPos, ss, alpha, beta, rootDepth);

		std::stable_sort(rootMoves.begin(), rootMoves.end());

		if (mainThread)
			CLI::printPV(rootPos, rootDepth, alpha, beta);

		for (int i = 0; i < 16; ++i) {
			Stack s = ss[i];
			std::cout << "ss[" << i << "]: ply=" << s.ply << ", pv=" << CLI::encode_move(s.pv) << "(" << s.pv << ")" << ", score=" << s.score << std::endl;
		}
		
		if (Threads.stop)
			completedDepth = rootDepth;

		if (bestValue >= VALUE_WIN)
			Threads.stop = true;
	}
}

namespace {

template <NodeType PT>
Value search(Position &pos, Stack *ss, Value alpha, Value beta, Depth depth) {

	const bool PvNode = (PT == PV);
	const bool rootNode = (PvNode && (ss - 1)->ply == 0);

	StateInfo st;
	Move move, pvMove;
	Value value, bestValue;
	int moveCount;

	Thread *thisThread = pos.this_thread();
	thisThread->nodes++;

	bool isMaximizing = (pos.side_to_move() == pos.side_ai());
	bestValue = (isMaximizing ? -VALUE_INFINITE : VALUE_INFINITE);

	ss->ply = (ss - 1)->ply + 1;

	if (thisThread == Threads.main() && Time.elapsed() > Time.maximum())
		Threads.stop = true;

	if (PvNode && thisThread->selDepth < ss->ply)
		thisThread->selDepth = ss->ply;

	if (!rootNode) {
		if (Threads.stop.load(std::memory_order_relaxed) || ss->ply >= MAX_PLY || depth <= DEPTH_ZERO)
			return pos.score();
	}
	
	Color winColor;
	if (!rootNode && pos.is_win(winColor))
		return (winColor == pos.side_ai() ? VALUE_WIN : VALUE_LOSE);

	
	pvMove = (PvNode ? ss->pv : MOVE_NONE);
	MovePicker mp(pos, pvMove);

	if (rootNode) {
		CLI::printPosition(pos);
	}

	moveCount = 0;
	while ((move = mp.next_move()) != MOVE_NONE) {
		
		if (!pos.legal(move))
			std::cout << "illegal: " << CLI::encode_move(move) << "(" << move << ")" << std::endl;

		pos.do_move(move, st);
		if(PvNode && moveCount <= 0)
			value = ::search<PV>(pos, ss + 1, alpha, beta, depth - ONE_PLY);
		else
			value = ::search<NonPV>(pos, ss + 1, alpha, beta, depth - ONE_PLY);
		pos.undo_move(move);

		++moveCount;		
		
		/*if ((isMaximizing && value > ss->score) || (!isMaximizing && value < ss->score)) {
			ss->pv = move;
			ss->score = value;
		}*/		
		
		if (PvNode && ((isMaximizing && value > bestValue) || (!isMaximizing && value < bestValue))) {
			ss->pv = move;
			std::memset(ss + 1, 0, (28 - ss->ply) * sizeof(Stack));
		}

		if (rootNode) {
			std::cout << CLI::encode_move(move) << std::endl;
			if (!std::count(thisThread->rootMoves.begin(), thisThread->rootMoves.end(), move)) {
				std::cout << "NOT FOUND: " << CLI::encode_move(move) << "(" << move << ")" << std::endl;
				std::cout << "ply=" << ss->ply << ", pv=" << CLI::encode_move(ss->pv) << "(" << move << ")" << ", score=" << ss->score << std::endl;
			}

			RootMove &rm = *std::find(thisThread->rootMoves.begin(), thisThread->rootMoves.end(), move);

			rm.score = value;
			rm.pv = move;
			rm.selDepth = thisThread->selDepth;

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

void CLI::printPV(Position &pos, Depth depth, Value alpha, Value beta) {
	int elapsed = Time.elapsed() + 1;
	const RootMoves &rootMoves = pos.this_thread()->rootMoves;
	uint64_t nodesSearched = Threads.nodes_searched();
	
	std::cout
		<< "depth "	<< depth / ONE_PLY
		<< "\tseldepth " << rootMoves[0].selDepth
		<< "\tscore "	<< rootMoves[0].score
		<< (rootMoves[0].score >= beta ? "\tlb" : (rootMoves[0].score <= alpha ? "\tup" : "\t"))
		<< "\tnodes "	<< nodesSearched
		<< "\tnps "		<< nodesSearched * 1000 / elapsed
		<< "\ttime "		<< elapsed
		<< "\tpv "		<< CLI::encode_move(rootMoves[0].pv)
		<< std::endl;
}