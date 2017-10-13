#pragma once

#define AI_NAME "Sockfish AI"
#define AI_VERSION "v0.2"

enum PieceColor {
	NO_PIECE_COLOR, 
	WHITE, RED, 
	PIECE_COLOR_NB
};

enum PieceType {
	NO_PIECE_TYPE,
	MINI_NINJA, MINI_SAMURAI, NINJA, SAMURAI, KING,
	PIECE_TYPE_NB
};

struct Piece {
	PieceColor color;
	PieceType type;
};

const char PieceTypeChar[PIECE_TYPE_NB] = {
	' ', 'j', 's', 'J', 'S', 'K'
};