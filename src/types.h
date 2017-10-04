#pragma once

enum Color {
	WHITE, RED, COLOR_NB
};

enum PieceType {
	NO_PIECE_TYPE,
	MINI_NINJA, MINI_SAMURAI, NINJA, SAMURAI, KING,
	PIECE_TYPE_NB
};

enum Piece {
	NO_PIECE,
	W_MINI_NINJA, W_MINI_SAMURAI, W_NINJA, W_SAMURAI, W_KING,
	R_MINI_NINJA, R_MINI_SAMURAI, R_NINJA, R_SAMURAI, R_KING,
	PIECE_NB
};

const char PieceTypeChar[PIECE_TYPE_NB] = {
	' ', 'j', 's', 'J', 'S', 'K'
};