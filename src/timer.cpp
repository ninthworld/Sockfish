#include <algorithm>

#include "search.h"
#include "timer.h"

Timer Time;

void Timer::init(TimePoint &st) {
	startTime = st;
	maximumTime = MAX_TIME;
}