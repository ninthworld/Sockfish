#pragma once

#include <vector>

#include "movegen.h"
#include "types.h"

class Position;

namespace Search {

struct Stack {
	Move *pv;
	int ply;
	Move currentMove;
	Move excludedMove;
	Move killers[2];
	Value staticEval;
	int statScore;
	int moveCount;
};

struct RootMove {
	RootMove(Move m) : pv(m) {}
	bool operator==(const Move &m) const { return pv == m; }
	bool operator<(const RootMove &m) const {
		return m.score != score ? m.score < score 
								: m.prevScore < prevScore;
	}

	Value score = -VALUE_INFINITE;
	Value prevScore = -VALUE_INFINITE;
	Move pv;
};

typedef std::vector<RootMove> RootMoves;

extern TimePoint StartTime;

void clear();

} // namespace Search
