#pragma once

#include <vector>

#include "types.h"

class Position;

struct Stack {
	Move pv;
	int ply;
	Move killers[2];
	Move currentMove;
};

namespace Search {

struct RootMove {
	RootMove(Move m) : pv(m) {}
	bool operator==(const Move &m) const { return pv == m; }
	bool operator<(const RootMove &m) const {
		return m.score != score ? m.score < score 
								: m.prevScore < prevScore;
	}

	Value score = -VALUE_INFINITE;
	Value prevScore = -VALUE_INFINITE;
	int selDepth;
	Move pv;
};

typedef std::vector<RootMove> RootMoves;

extern TimePoint StartTime;

void clear();

} // namespace Search
