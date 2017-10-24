#pragma once

#include "types.h"

struct TTEntry {

	Move  move()  const { return (Move)move16; }
	Value value() const { return (Value)value16; }
	Value eval()  const { return (Value)eval16; }
	Depth depth() const { return (Depth)(depth8 * int(ONE_PLY)); }
	Bound bound() const { return (Bound)(genBound8 & 0x3); }

	void save(Key k, Value v, Bound b, Depth d, Move m, Value ev, uint8_t g) {
		
		if (m || (k >> 48) != key16)
			move16 = (uint16_t)m;

		if ((k >> 48) != key16
			|| d / ONE_PLY > depth8 - 4
			|| b == BOUND_EXACT) {
			key16 = (uint16_t)(k >> 48);
			value16 = (int16_t)v;
			eval16 = (int16_t)ev;
			genBound8 = (uint8_t)(g | b);
			depth8 = (int8_t)(d / ONE_PLY);
		}
	}

private:
	friend class TranspositionTable;

	uint16_t key16;
	uint16_t move16;
	int16_t  value16;
	int16_t  eval16;
	uint8_t  genBound8;
	int8_t   depth8;
};

class TranspositionTable {
	
	static const int CacheLineSize = 64;
	static const int ClusterSize = 3;

	struct Cluster {
		TTEntry entry[ClusterSize];
		char padding[2];
	};

	static_assert(CacheLineSize % sizeof(Cluster) == 0, "Cluster size incorrect");

public:
	~TranspositionTable() { free(mem); }
	void new_search() { generation8 += 4; is_empty = false; }
	uint8_t generation() const { return generation8; }
	TTEntry *probe(const Key key, bool &found) const;
	int hashfull() const;
	void resize(size_t mbSize);
	void clear();

	TTEntry *first_entry(const Key key) const {
		return &table[(size_t)key & (clusterCount - 1)].entry[0];
	}

	bool is_empty = true;
private:
	size_t clusterCount;
	Cluster *table;
	void *mem;
	uint8_t generation8;
};

extern TranspositionTable TT;