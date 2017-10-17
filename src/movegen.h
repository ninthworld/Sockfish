#pragma once

#include <algorithm>

#include "types.h"

class Position;

struct ExtMove {
	Move move;
	int value;

	operator Move() const { return move; }
	void operator=(Move m) { move = m; }
};

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

class MovePicker {
public:
	MovePicker(const Position &p);
	Move next_move();

private:
	ExtMove* begin() { return cur; }
	ExtMove* end() { return endMoves; }

	const Position &pos;
	ExtMove *cur, *endMoves;
	ExtMove moves[MAX_MOVES];
};