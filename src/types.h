#pragma once

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <chrono>

/*
Stockfish, a UCI chess playing engine derived from Glaurung 2.1
Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

Modified code snippet from Stockfish <types.h> and <misc.h>
<modified_code>
*/
typedef uint64_t Key;
typedef uint64_t Bitboard;
typedef std::chrono::milliseconds::rep TimePoint;

const TimePoint MAX_TIME = 4900; // 4.9 seconds
const int MAX_MOVES = 256;
const int MAX_PLY = 128;

inline TimePoint now() {
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::steady_clock::now().time_since_epoch()).count();
}

inline bool max_time(TimePoint start) {
	return (now() - start) >= MAX_TIME;
}
/*
</modified_code>
*/

/*
Heavily modifed code snippet from Stockfish <types.h>
Specifically: Colors, Pieces, Scores, Squares have all been changed.

<modified_code>
*/
enum Move : int {
	MOVE_NONE,
	MOVE_NULL = 65
};

enum Color {
	WHITE, RED, COLOR_NB = 2
};

enum Bound {
	BOUND_NONE,
	BOUND_UPPER,
	BOUND_LOWER,
	BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
};

enum Value : int {
	VALUE_ZERO = 0,
	VALUE_INFINITE = 32001,
	VALUE_NONE = 32002,

	VALUE_WIN = 32000,
	VALUE_LOSE = -32000,

	MiniNinjaValue			= 100,
	MiniSamuraiValue		= 80,
	NinjaValue				= 800,
	SamuraiValue			= 640,

	MiniNinjaModifier		= 2,
	MiniSamuraiModifier		= 2,
	NinjaModifier			= 2,
	SamuraiModifier			= 2,

	MoveCountValue			= 80
};

enum PieceType {
	NO_PIECE_TYPE, MINI_NINJA, MINI_SAMURAI, NINJA, SAMURAI, KING,
	ALL_PIECES = 0,
	PIECE_TYPE_NB = 7
};

enum Piece {
	NO_PIECE,
	W_MINI_NINJA = 1, W_MINI_SAMURAI, W_NINJA, W_SAMURAI, W_KING,
	R_MINI_NINJA = 9, R_MINI_SAMURAI, R_NINJA, R_SAMURAI, R_KING,
	PIECE_NB = 16
};

extern Value PieceValue[PIECE_NB];

enum Depth : int {
	ONE_PLY = 1,
	DEPTH_ZERO = 0,
	DEPTH_NONE = -6,
	DEPTH_MAX = MAX_PLY
};

/*
NOTE: Files and Ranks have been inverted.
This was done to accomodate a 7x8 board size
while maintaining the divisibility by base 2
*/
enum Square {
	SQ_A1, SQ_A2, SQ_A3, SQ_A4, SQ_A5, SQ_A6, SQ_A7, SQ_A8,
	SQ_B1, SQ_B2, SQ_B3, SQ_B4, SQ_B5, SQ_B6, SQ_B7, SQ_B8,
	SQ_C1, SQ_C2, SQ_C3, SQ_C4, SQ_C5, SQ_C6, SQ_C7, SQ_C8,
	SQ_D1, SQ_D2, SQ_D3, SQ_D4, SQ_D5, SQ_D6, SQ_D7, SQ_D8,
	SQ_E1, SQ_E2, SQ_E3, SQ_E4, SQ_E5, SQ_E6, SQ_E7, SQ_E8,
	SQ_F1, SQ_F2, SQ_F3, SQ_F4, SQ_F5, SQ_F6, SQ_F7, SQ_F8,
	SQ_G1, SQ_G2, SQ_G3, SQ_G4, SQ_G5, SQ_G6, SQ_G7, SQ_G8,
	SQ_NONE,
	SQUARE_NB = 56,
	NORTH = 1,
	EAST = 8,
	SOUTH = -1,
	WEST = -8,
	NORTH_EAST = 9,
	SOUTH_EAST = 7,
	NORTH_WEST = -7,
	SOUTH_WEST = -9
};

enum File : int {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_NB
};

enum Rank : int {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};

enum Score : int {
	SCORE_ZERO
};

#define ENABLE_BASE_OPERATORS_ON(T)                             \
inline T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
inline T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
inline T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }      \
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }      

#define ENABLE_FULL_OPERATORS_ON(T)                             \
ENABLE_BASE_OPERATORS_ON(T)                                     \
inline T operator*(int i, T d) { return T(i * int(d)); }        \
inline T operator*(T d, int i) { return T(int(d) * i); }        \
inline T operator*(T d, T i) { return T(int(d) * int(i)); }        \
inline T& operator++(T& d) { return d = T(int(d) + 1); }        \
inline T& operator--(T& d) { return d = T(int(d) - 1); }        \
inline T operator/(T d, int i) { return T(int(d) / i); }        \
inline int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); } \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); } \
inline T operator^(T d1, T d2) { return T((int)d1 ^ (int)d2); }

ENABLE_FULL_OPERATORS_ON(Value)
ENABLE_FULL_OPERATORS_ON(PieceType)
ENABLE_FULL_OPERATORS_ON(Piece)
ENABLE_FULL_OPERATORS_ON(Color)
ENABLE_FULL_OPERATORS_ON(Depth)
ENABLE_FULL_OPERATORS_ON(Square)
ENABLE_FULL_OPERATORS_ON(File)
ENABLE_FULL_OPERATORS_ON(Rank)

#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

inline Value operator+(Value v, int i) { return Value(int(v) + i); }
inline Value operator-(Value v, int i) { return Value(int(v) - i); }
inline Value& operator+=(Value& v, int i) { return v = v + i; }
inline Value& operator-=(Value& v, int i) { return v = v - i; }

inline Color operator~(Color c) {
	return Color(c ^ RED);
}

inline Square operator~(Square s) {
	return Square(s ^ SQ_A8);
}

inline Piece operator~(Piece pc) {
	return Piece(pc ^ 8);
}

inline Square make_square(File f, Rank r) {
	return Square((f << 3) + r);
}

inline Piece make_piece(Color c, PieceType pt) {
	return Piece((c << 3) + pt);
}

inline PieceType type_of(Piece pc) {
	return PieceType(pc & 7);
}

inline Color color_of(Piece pc) {
	return Color(pc >> 3);
}

inline bool is_ok(Square s) {
	return s >= SQ_A1 && s <= SQ_G8;
}

inline File file_of(Square s) {
	return File(s >> 3);
}

inline Rank rank_of(Square s) {
	return Rank(s & 7);
}

inline Square from_sq(Move m) {
	return Square((m >> 6) & 0x3F);
}

inline Square to_sq(Move m) {
	return Square(m & 0x3F);
}

inline int from_to(Move m) {
	return m & 0xFFF;
}

inline Move make_move(Square from, Square to) {
	return Move((from << 6) + to);
}

inline bool is_ok(Move m) {
	return from_sq(m) != to_sq(m);
}

inline Square invert(Square s) {
	return make_square(FILE_G - file_of(s), RANK_8 - rank_of(s));
}

inline Move invert(Move m) {
	return make_move(invert(from_sq(m)), invert(to_sq(m)));
}
/*
</modified_code>
*/

namespace ValueMap {

const int Modifier[PIECE_TYPE_NB] = {
	0,
	MiniNinjaModifier * MiniNinjaValue / 10,
	MiniSamuraiModifier * MiniSamuraiValue / 10,
	NinjaModifier * NinjaValue / 10,
	SamuraiModifier * SamuraiValue / 10
};
extern int Values[PIECE_NB][SQUARE_NB];

void init();

} // namespace ValueMap