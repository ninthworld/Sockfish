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

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "types.h"
#include "position.h"
#include "search.h"

#define THREAD_MOVECOUNT_NB 6

class Thread {

	std::mutex mutex;
	std::condition_variable cv;
	size_t id;
	bool exit = false, searching = true;
	std::thread stdThread;

public:
	Thread(size_t id);
	virtual ~Thread();
	virtual void search();
	void clear();
	void idle_loop();
	void start_searching();
	void wait_for_search_finished();

	int selDepth;
	Position rootPos;
	Search::RootMoves rootMoves;
	Depth rootDepth, completedDepth;
	std::atomic<uint64_t> nodes;
	std::atomic<uint64_t> ttHits;
	std::atomic<uint64_t> ttSaves;
	
	/*
	NOTE: These are the main additions to this file not found in Stockfish
	*/
	int moveCount[THREAD_MOVECOUNT_NB];
	Move counterMoves[4096];
	Value history[COLOR_NB][4096][32];
};

struct MainThread : public Thread {
	using Thread::Thread;

	void search() override;

	Value prevScore;
};

struct ThreadPool : public std::vector<Thread*> {
	
	void init(size_t requested);
	void exit();
	void start_thinking(Position &pos, StateListPtr &states, const TimePoint &st);
	void set(size_t requested);

	MainThread* main() const { return static_cast<MainThread*>(front()); }
	uint64_t nodes_searched() const { return accumulate(&Thread::nodes); }
	Thread* best_thread() const { return static_cast<Thread*>(bestThread); }

	std::atomic_bool stop;
	Thread *bestThread;

private:
	StateListPtr setupStates;

	uint64_t accumulate(std::atomic<uint64_t> Thread::* member) const {
		uint64_t sum = 0;
		for (Thread* th : *this)
			sum += (th->*member).load(std::memory_order_relaxed);
		return sum;
	}
};

extern ThreadPool Threads;