#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

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
	
	Position rootPos;
	Search::RootMoves rootMoves;
	Depth rootDepth, completedDepth;
	std::atomic<uint64_t> nodes;
};

struct MainThread : public Thread {
	using Thread::Thread;

	void search() override;

	Value prevScore;
};

struct ThreadPool : public std::vector<Thread*> {
	
	void init(size_t requested);
	void exit();
	void start_thinking(Position &pos, StateListPtr& states, const TimePoint &st);
	void set(size_t requested);

	MainThread* main() const { return static_cast<MainThread*>(front()); }
	Thread* best_thread() const { return static_cast<Thread*>(bestThread); }

	std::atomic_bool stop;
	Thread *bestThread;

private:
	StateListPtr setupStates;
};

extern ThreadPool Threads;