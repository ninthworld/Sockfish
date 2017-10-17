#include <algorithm>

#include "bitboard.h"

uint8_t PopCnt16[1 << 16];
int SquareDistance[SQUARE_NB][SQUARE_NB];

Bitboard SquareBB[SQUARE_NB];
Bitboard FileBB[FILE_NB];
Bitboard RankBB[RANK_NB];
Bitboard RankFillBB[RANK_NB];
Bitboard MiniMovesBB[PIECE_NB][SQUARE_NB];
Bitboard MiniAttacksBB[PIECE_NB][SQUARE_NB];

Magic NinjaMagics[SQUARE_NB];
Magic SamuraiMagics[SQUARE_NB];

namespace {

const uint64_t DeBruijn64 = 0x3F79D71B4CB0A89ULL;
Square BSFTable[64];

Bitboard NinjaTable[0x1480];
Bitboard SamuraiTable[0x19000];

void init_magics(Bitboard table[], Magic magics[], Square deltas[]);

unsigned bsf_index(Bitboard b) {
	b ^= b - 1;
	return (b * DeBruijn64) >> 58;
}

unsigned popcount16(unsigned u) {
	u -= (u >> 1) & 0x5555U;
	u = ((u >> 2) & 0x3333U) + (u & 0x3333U);
	u = ((u >> 4) + u) & 0x0F0FU;
	return (u * 0x0101U) >> 8;
}

} // namespace

Square lsb(Bitboard b) {
	return BSFTable[bsf_index(b)];
}

void Bitboards::initBBs() {

	for (unsigned i = 0; i < (1 << 16); ++i)
		PopCnt16[i] = (uint8_t)popcount16(i);

	for (Square s = SQ_A1; s <= SQ_G8; ++s) {
		SquareBB[s] = 1ULL << s;
		BSFTable[bsf_index(SquareBB[s])] = s;
	}

	for (File f = FILE_A; f <= FILE_G; ++f)
		FileBB[f] = f > FILE_A ? FileBB[f - 1] << 8 : FileABB;

	for (Rank r = RANK_1; r <= RANK_8; ++r)
		RankBB[r] = r > RANK_1 ? RankBB[r - 1] << 1 : Rank1BB;

	for (Rank r1 = RANK_1; r1 <= RANK_8; ++r1) {
		RankFillBB[r1] = RankBB[r1];
		for (Rank r2 = RANK_1; r2 < r1; ++r2)
			RankFillBB[r1] |= RankBB[r2];
	}

	for (Square s1 = SQ_A1; s1 <= SQ_G8; ++s1)
		for (Square s2 = SQ_A1; s2 <= SQ_G8; ++s2)
			if (s1 != s2)
				SquareDistance[s1][s2] = std::max(distance<File>(s1, s2), distance<Rank>(s1, s2));


	for (Rank r = RANK_1; r <= RANK_8; ++r)
		for (File f = FILE_A; f <= FILE_G; ++f) {
			Square s = make_square(f, r);
			Bitboard sBB = SquareBB[s];

			// Mini Ninja Moves
			MiniMovesBB[W_MINI_NINJA][s] |= shift<NORTH_EAST>(sBB);
			MiniMovesBB[W_MINI_NINJA][s] |= shift<NORTH_WEST>(sBB);
			MiniMovesBB[R_MINI_NINJA][s] |= shift<SOUTH_EAST>(sBB);
			MiniMovesBB[R_MINI_NINJA][s] |= shift<SOUTH_WEST>(sBB);
			
			// Mini Samurai Moves
			MiniMovesBB[W_MINI_SAMURAI][s] |= shift<NORTH>(sBB);
			MiniMovesBB[R_MINI_SAMURAI][s] |= shift<SOUTH>(sBB);

			// Mini Ninja Attacks
			MiniAttacksBB[W_MINI_NINJA][s] |= shift<SOUTH_EAST>(sBB);
			MiniAttacksBB[W_MINI_NINJA][s] |= shift<SOUTH_WEST>(sBB);
			MiniAttacksBB[R_MINI_NINJA][s] |= shift<NORTH_EAST>(sBB);
			MiniAttacksBB[R_MINI_NINJA][s] |= shift<NORTH_WEST>(sBB);

			// Mini Samurai Attacks
			MiniAttacksBB[W_MINI_SAMURAI][s] |= shift<EAST>(sBB);
			MiniAttacksBB[W_MINI_SAMURAI][s] |= shift<WEST>(sBB);
			MiniAttacksBB[R_MINI_SAMURAI][s] |= shift<EAST>(sBB);
			MiniAttacksBB[R_MINI_SAMURAI][s] |= shift<WEST>(sBB);
		}
}

void Bitboards::initMagicBBs() {

	Square NinjaDeltas[] = { NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST };
	Square SamuraiDeltas[] = { NORTH, EAST, SOUTH, WEST };

	init_magics(NinjaTable, NinjaMagics, NinjaDeltas);
	init_magics(SamuraiTable, SamuraiMagics, SamuraiDeltas);

}

namespace {

Bitboard sliding_attack(Square deltas[], Square sq, Bitboard occupied) {
	Bitboard attack = 0;
	for (int i = 0; i < 4; ++i) {
		for (Square s = sq + deltas[i]; is_ok(s) && distance(s, s - deltas[i]) == 1; s += deltas[i]) {
			if (occupied & s)
				break;
			attack |= s; 
		}
	}

	return range_mask(attack);
}

void init_magics(Bitboard table[], Magic magics[], Square deltas[]) {
	int seeds[RANK_NB] = { 728, 10316, 55013, 32803, 12281, 15100, 16645, 255 };

	Bitboard occupancy[4096], reference[4096], edges, b;
	int epoch[4096] = {}, cnt = 0, size = 0;

	for (Square s = SQ_A1; s <= SQ_G8; ++s) {
		edges = (((Rank1BB | Rank8BB) & ~rank_bb(s)) | ((FileABB | FileGBB) & ~file_bb(s)));

		Magic &m = magics[s];
		m.mask = sliding_attack(deltas, s, 0) & ~edges;
		m.shift = 64 - popcount(m.mask);

		m.attacks = (s == SQ_A1 ? table : magics[s - 1].attacks + size);

		b = size = 0;
		do {
			occupancy[size] = b;
			reference[size] = sliding_attack(deltas, s, b);
			
			size++;
			b = ((b - m.mask) & m.mask);
		} while (b);
		
		PRNG rng(seeds[rank_of(s)]);

		for (int i = 0; i < size;) {
			for (m.magic = 0; popcount((m.magic * m.mask) >> 56) < 6;) {
				m.magic = rng.sparse_rand<Bitboard>();

				for (++cnt, i = 0; i < size; ++i) {
					unsigned idx = m.index(occupancy[i]);

					if (epoch[idx] < cnt) {
						epoch[idx] = cnt;
						m.attacks[idx] = reference[i];
					}
					else if (m.attacks[idx] != reference[i])
						break;
				}
			}
		}
	}
}

} // namespace