#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "types.h"
#include "position.h"
#include "search.h"

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