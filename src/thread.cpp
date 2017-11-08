/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Stockfish is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Stockfish is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
NOTE: This code has been slightly altered from it's original form, but performs
all the same tasks as its predecessor.
*/

#include <algorithm>

#include "search.h"
#include "thread.h"
#include "movegen.h"

ThreadPool Threads;

Thread::Thread(size_t id) : id(id), stdThread(&Thread::idle_loop, this) {
	wait_for_search_finished();
	clear();
}

Thread::~Thread() {
	exit = true;
	start_searching();
	stdThread.join();
}

void Thread::clear() {
	std::memset(counterMoves, MOVE_NONE, 4096 * sizeof(Move));
	std::memset(history, VALUE_ZERO, 2 * 4096 * 32 * sizeof(Value));
}

void Thread::start_searching() {
	std::lock_guard<std::mutex> lk(mutex);
	searching = true;
	cv.notify_one();
}

void Thread::wait_for_search_finished() {
	std::unique_lock<std::mutex> lk(mutex);
	cv.wait(lk, [&]{ return !searching; });
}

void Thread::idle_loop() {
	while (true) {
		std::unique_lock<std::mutex> lk(mutex);
		searching = false;
		cv.notify_one();
		cv.wait(lk, [&] { return searching; });

		if (exit)
			return;

		lk.unlock();

		search();
	}
}

void ThreadPool::init(size_t requested) {
	push_back(new MainThread(0));
	set(requested);
}

void ThreadPool::exit() {
	main()->wait_for_search_finished();
	set(0);
}

void ThreadPool::set(size_t requested) {
	while (size() < requested)
		push_back(new Thread(size()));

	while (size() > requested)
		delete back(), pop_back();
}

/*
NOTE: This function has been slightly altered from its original form.
*/
void ThreadPool::start_thinking(Position &pos, StateListPtr &states, const TimePoint &st) {

	main()->wait_for_search_finished();

	stop = false;

	Search::StartTime = st;
	Search::RootMoves rootMoves;

	for (const auto& m : MoveList(pos))
		rootMoves.emplace_back(m);

	if (states.get())
		setupStates = std::move(states);

	StateInfo tmp = setupStates->back();

	for (Thread *th : Threads) {
		th->nodes = 0;
		th->ttHits = 0;
		th->ttSaves = 0;
		std::memset(th->moveCount, 0, THREAD_MOVECOUNT_NB * sizeof(int));
		th->rootDepth = th->completedDepth = DEPTH_ZERO;
		th->rootMoves = rootMoves;
		th->rootPos.set(pos, &setupStates->back(), th);
	}

	setupStates->back() = tmp;

	main()->start_searching();
}