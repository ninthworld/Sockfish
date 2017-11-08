#pragma once

#include <vector>

#include "types.h"

class Position;

/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Modified code snippet from Stockfish <search.h>
<modified_code>
*/
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
/*
</modified_code>
*/

extern TimePoint StartTime;

void clear();

} // namespace Search
