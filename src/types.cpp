#include "types.h"

namespace ValueMap {

int Values[PIECE_NB][SQUARE_NB] = {
	{},
	{	// W_MINI_NINJA
		//	1	2	3	4	5	6	7	8	
			0,	0,	0,	7,	0,	7,	0,	0, // A
			0,	0,	0,	0,	8,	0,	8,	0, // B
			0,	0,	0,	7,	0,	9,	0,	9, // C
			0,	0,	0,	0,	8,	0, 10,	0, // D
			0,	0,	0,	7,	0,	9,	0,	9, // E
			0,	0,	0,	0,	8,	0,	8,	0, // F
			0,	0,	0,	7,	0,	7,	0,	0  // G
		//	1	2	3	4	5	6	7	8	
	},
	{	// W_MINI_SAMURAI
		//	1	2	3	4	5	6	7	8	
			0,	0,	0,	0,	0,	0,	0, -9, // A
			0,	0,	0,	0,	0,	0,	0, -9, // B
			0,	0,	0,	6,	7,	8,	9, -9, // C
			0,	0,	6,	7,	8,	9, 10,	0, // D
			0,	0,	0,	6,	7,	8,	9, -9, // E
			0,	0,	0,	0,	0,	0,	0, -9, // F
			0,	0,	0,	0,	0,	0,	0, -9  // G
	   //	1	2	3	4	5	6	7	8	
	},
	{	// W_NINJA
		//	1	2	3	4	5	6	7	8	
			0,	0,	0,	7,	0,	7,	0,	0, // A
			0,	0,	6,	0,	8,	0,	8,	0, // B
			0,	0,	0,	7,	0,	9,	0,	9, // C
			0,	0,	6,	0,	8,	0, 10,	0, // D
			0,	0,	0,	7,	0,	9,	0,	9, // E
			0,	0,	6,	0,	8,	0,	8,	0, // F
			0,	0,	0,	7,	0,	7,	0,	0  // G
		//	1	2	3	4	5	6	7	8	
	},
	{	// W_SAMURAI
		//	1	2	3	4	5	6	7	8	
			0,	0,	3,	4,	5,	6,	7, -9, // A
			0,	0,	4,	5,	6,	7,	8, -9, // B
			0,	0,	5,	6,	7,	8,	9, -9, // C
			0,	0,	6,	7,	8,	9, 10,	0, // D
			0,	0,	5,	6,	7,	8,	9, -9, // E
			0,	0,	4,	5,	6,	7,	8, -9, // F
			0,	0,	3,	4,	5,	6,	7, -9  // G
		 //	1	2	3	4	5	6	7	8	
	},
	{},
	{},
	{},
	{},
	{	// R_MINI_NINJA
		//	1	2	3	4	5	6	7	8	
			0,	0, -7,	0, -7,	0,	0,	0, // A
			0, -8,	0, -8,	0,	0,	0,	0, // B
			-9,	0, -9,	0, -7,	0,	0,	0, // C
			0,-10,	0, -8,	0,	0,	0,	0, // D
			-9,	0, -9,	0, -7,	0,	0,	0, // E
			0, -8,	0, -8,	0,	0,	0,	0, // F
			0,	0, -7,	0, -7,	0,	0,	0  // G
		 //	1	2	3	4	5	6	7	8	
	},
	{	// R_MINI_SAMURAI
		//	1	2	3	4	5	6	7	8	
			9,	0,	0,	0,	0,	0,	0,	0, // A
			9,	0,	0,	0,	0,	0,	0,	0, // B
			9, -9, -8, -7, -6,	0,	0,	0, // C
			0,-10, -9, -8, -7,	0,	0,	0, // D
			9, -9, -8, -7, -6,	0,	0,	0, // E
			9,	0,	0,	0,	0,	0,	0,	0, // F
			9,	0,	0,	0,	0,	0,	0,	0  // G
	  //	1	2	3	4	5	6	7	8	
	},
	{	// R_NINJA
		//	1	2	3	4	5	6	7	8	
			0,	0, -7,	0, -7,	0,	0,	0, // A
			0, -8,	0, -8,	0, -6,	0,	0, // B
			-9,	0, -9,	0, -7,	0,	0,	0, // C
			0,-10,	0, -8,	0, -6,	0,	0, // D
			-9,	0, -9,	0, -7,	0,	0,	0, // E
			0, -8,	0, -8,	0, -6,	0,	0, // F
			0,	0, -7,	0, -7,	0,	0,	0  // G
	  //	1	2	3	4	5	6	7	8	
	},
	{	// R_SAMURAI
		//	1	2	3	4	5	6	7	8	
			9, -7, -6, -5, -4, -3,	0,	0, // A
			9, -8, -7, -6, -5, -4,	0,	0, // B
			9, -9, -8, -7, -6, -5,	0,	0, // C
			0,-10, -9, -8, -7, -6,	0,	0, // D
			9, -9, -8, -7, -6, -5,	0,	0, // E
			9, -8, -7, -6, -5, -4,	0,	0, // F
			9, -7, -6, -5, -4, -3,	0,	0  // G
		 //	1	2	3	4	5	6	7	8	
	}
};

void init() {
	for (Color c = WHITE; c <= RED; ++c) {
		for (PieceType pt = MINI_NINJA; pt < KING; ++pt) {
			Piece p = make_piece(c, pt);
			for (Square s = SQ_A1; s <= SQ_G8; ++s) {
				Values[p][s] *= Modifier[pt];
			}
		}
	}
}

} // namespace ValueMap