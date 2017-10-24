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
		th->rootDepth = th->completedDepth = DEPTH_ZERO;
		th->rootMoves = rootMoves;
		th->rootPos.set(pos, &setupStates->back(), th);
	}

	setupStates->back() = tmp;

	main()->start_searching();
}