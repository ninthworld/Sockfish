#pragma once

#include "search.h"
#include "thread.h"

class Timer {
public:
	void init(TimePoint &st);
	int maximum() const { return maximumTime; }
	int elapsed() const { return int(now() - startTime); }

private:
	TimePoint startTime;
	int maximumTime;
};

extern Timer Time;