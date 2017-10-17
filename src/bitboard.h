#pragma once

#include <string>

#include "types.h"

namespace Bitboards {

void initBBs();
void initMagicBBs();

} // namespace Bitboards

const Bitboard AllSquares = ~Bitboard(0);
const Bitboard DarkSquares = 0x55AA55AA55AA55ULL;
const Bitboard RangeSquares = 0xFFFFFFFFFFFFFFULL;

const Bitboard FileABB = 0xFF;
const Bitboard FileBBB = FileABB << (8 * 1);
const Bitboard FileCBB = FileABB << (8 * 2);
const Bitboard FileDBB = FileABB << (8 * 3);
const Bitboard FileEBB = FileABB << (8 * 4);
const Bitboard FileFBB = FileABB << (8 * 5);
const Bitboard FileGBB = FileABB << (8 * 6);

const Bitboard Rank1BB = 0x01010101010101ULL;
const Bitboard Rank2BB = Rank1BB << 1;
const Bitboard Rank3BB = Rank1BB << 2;
const Bitboard Rank4BB = Rank1BB << 3;
const Bitboard Rank5BB = Rank1BB << 4;
const Bitboard Rank6BB = Rank1BB << 5;
const Bitboard Rank7BB = Rank1BB << 6;
const Bitboard Rank8BB = Rank1BB << 7;

extern int SquareDistance[SQUARE_NB][SQUARE_NB];

extern Bitboard SquareBB[SQUARE_NB];
extern Bitboard FileBB[FILE_NB];
extern Bitboard RankBB[RANK_NB];
extern Bitboard RankFillBB[RANK_NB];
extern Bitboard MiniMovesBB[PIECE_NB][SQUARE_NB];
extern Bitboard MiniAttacksBB[PIECE_NB][SQUARE_NB];

struct Magic {
	Bitboard mask;
	Bitboard magic;
	Bitboard* attacks;
	unsigned shift;

	unsigned index(Bitboard occupied) const {
		return unsigned(((occupied & mask) * magic) >> shift);
	};
};

extern Magic NinjaMagics[SQUARE_NB];
extern Magic SamuraiMagics[SQUARE_NB];

inline Bitboard operator&(Bitboard b, Square s) {
	return b & SquareBB[s];
}

inline Bitboard operator|(Bitboard b, Square s) {
	return b | SquareBB[s];
}

inline Bitboard operator^(Bitboard b, Square s) {
	return b ^ SquareBB[s];
}

inline Bitboard& operator|=(Bitboard& b, Square s) {
	return b |= SquareBB[s];
}

inline Bitboard& operator^=(Bitboard& b, Square s) {
	return b ^= SquareBB[s];
}

inline bool more_than_one(Bitboard b) {
	return b & (b - 1);
}

inline Bitboard rank_bb(Rank r) {
	return RankBB[r];
}

inline Bitboard rank_bb(Square s) {
	return RankBB[rank_of(s)];
}

inline Bitboard file_bb(File f) {
	return FileBB[f];
}

inline Bitboard file_bb(Square s) {
	return FileBB[file_of(s)];
}

inline Bitboard square_bb(Square s) {
	return SquareBB[s];
}

inline Bitboard range_mask(Bitboard b) {
	return b & RangeSquares;
}

template<Square D>
inline Bitboard shift(Bitboard b) {
	switch (D) {
	case NORTH:
		return (b & ~Rank8BB) << 1;
	case SOUTH:
		return (b & ~Rank1BB) >> 1;
	case EAST:
		return (b & ~FileGBB) << 8;
	case WEST:
		return (b & ~FileABB) >> 8;
	case NORTH_EAST:
		return (((b & ~Rank8BB) << 1) & ~FileGBB) << 8;
	case SOUTH_EAST:
		return (((b & ~Rank1BB) >> 1) & ~FileGBB) << 8;
	case NORTH_WEST:
		return (((b & ~Rank8BB) << 1) & ~FileABB) >> 8;
	case SOUTH_WEST:
		return (((b & ~Rank1BB) >> 1) & ~FileABB) >> 8;
	default:
		return 0;
	}
}

template<typename T> inline int distance(T x, T y) { return x < y ? y - x : x - y; }
template<> inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }

template<typename T1, typename T2> inline int distance(T2 x, T2 y);
template<> inline int distance<File>(Square x, Square y) { return distance(file_of(x), file_of(y)); }
template<> inline int distance<Rank>(Square x, Square y) { return distance(rank_of(x), rank_of(y)); }

inline Bitboard moves_bb(Square s, Piece p, Bitboard occupied) {
	if (p == NO_PIECE)
		return 0;

	Color c = color_of(p);
	PieceType pt = type_of(p);

	if (pt == KING)
		return 0;

	if (pt == MINI_NINJA || pt == MINI_SAMURAI)
		return MiniMovesBB[p][s] & (~occupied);

	Bitboard rfBB = RankFillBB[rank_of(s)];
	const Magic &m = (pt == NINJA ? NinjaMagics[s] : SamuraiMagics[s]);
	unsigned index = m.index(occupied);
	return (m.attacks[index] & ~occupied) & (c == WHITE ? ~rfBB : (rfBB ^ rank_bb(rank_of(s))));
}

inline Bitboard attacks_bb(Square s, Piece p, Bitboard whiteOccupied, Bitboard redOccupied) {
	if (p == NO_PIECE)
		return 0;

	Color c = color_of(p);
	PieceType pt = type_of(p);

	if (pt == KING)
		return 0;

	Bitboard occupied = (whiteOccupied | redOccupied);
	Bitboard attackBB;
	if (pt == MINI_NINJA || pt == MINI_SAMURAI) {
		attackBB = MiniAttacksBB[p][s] & ~occupied;
	}
	else {
		Bitboard rfBB = RankFillBB[rank_of(s)];
		const Magic &m = (pt == NINJA ? NinjaMagics[s] : SamuraiMagics[s]);
		unsigned index = m.index(occupied);
		attackBB = (m.attacks[index] & ~occupied) & (pt == NINJA ? (c == WHITE ? rfBB : ~rfBB) : rank_bb(rank_of(s)));
	}

	return attackBB & (c == WHITE ? shift<SOUTH>(redOccupied) : shift<NORTH>(whiteOccupied));
};

inline int popcount(Bitboard b) {
	extern uint8_t PopCnt16[1 << 16];
	union { Bitboard bb; uint16_t u[4]; } v = { b };
	return PopCnt16[v.u[0]] + PopCnt16[v.u[1]] + PopCnt16[v.u[2]] + PopCnt16[v.u[3]];
}

Square lsb(Bitboard b);
Square msb(Bitboard b);

inline Square pop_lsb(Bitboard* b) {
	const Square s = lsb(*b);
	*b &= *b - 1;
	return s;
}


class PRNG {
	uint64_t s;
	uint64_t rand64() {
		s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
		return s * 2685821657736338717LL;
	}

public:
	PRNG(uint64_t seed) : s(seed) { }

	template<typename T> T rand() {
		return T(rand64());
	}

	template<typename T> T sparse_rand() {
		return T(rand64() & rand64() & rand64());
	}
};