#pragma once

#include <vector>

#include "movegen.h"
#include "types.h"

class Position;

namespace Search {

Move best_move(Position &rootPos, TimePoint &start, Depth *maxDepth);

} // namespace Search
