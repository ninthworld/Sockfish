#pragma once

#include <algorithm>

#include "types.h"
#include "search.h"

class Position;
class Thread;

/*
Inspired by Stockfish
*/
struct ExtMove {
	Move move;
	Value score = -VALUE_INFINITE;
	Value prevScore = -VALUE_INFINITE;

	operator Move() const { return move; }
	void operator=(Move m) { move = m; }
	bool operator==(const Move &m) const { return move == m; }
	bool operator<(const ExtMove &m) const {
		return m.score != score ? m.score < score
			: m.prevScore < prevScore;
	}
};

/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Modified code snippet from Stockfish <movegen.h>
<modified_code>
*/
ExtMove* generate(const Position &pos, ExtMove *moveList);

struct MoveList {
	MoveList(const Position &pos) : last(generate(pos, moveList)) {}
	
	const ExtMove* begin() const { return moveList; }
	const ExtMove* end() const { return last; }
	
	size_t size() const { return last - moveList; }
	bool contains(Move move) const {
		return std::find(begin(), end(), move) != end();
	}

private:
	ExtMove moveList[MAX_MOVES], *last;
};
/*
</modified_code>
*/

/*
Modified from Stockfish <movepick.h>
<modified_code>
*/
class MovePicker {
public:

	MovePicker(const Position &p, const int ply, const Move currentMove, const Move ttMove, const Move killers[], Thread *thread);
	Move next_move();

private:
	ExtMove* begin() { return cur; }
	ExtMove* end() { return endMoves; }

	const Position &pos;
	ExtMove *cur, *endMoves;
	ExtMove moves[MAX_MOVES];
};
/*
</modified_code>
*/