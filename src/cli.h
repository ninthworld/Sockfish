#pragma once

#include <map>
#include <string>

#include "types.h"

class Position;

namespace CLI {

void init();
void loop();
void printPosition(Position &pos);
void printBitboard(Bitboard bb);
void printPV(Position &pos, Depth depth, Value alpha, Value beta);
bool promptYesNo(std::string str);

std::string encode_square(Square s);
std::string encode_move(Move m);
Square decode_square(std::string str);
Move decode_move(std::string str);

} // namespace CLI


inline std::string CLI::encode_square(Square s) {
	std::string str;

	str.push_back((char)file_of(s) + 'a');
	str.push_back((char)rank_of(s) + '1');

	return str;
}

inline std::string CLI::encode_move(Move m) {
	std::string str = encode_square(from_sq(m));
	str.append(encode_square(to_sq(m)));

	return str;
}


inline Square CLI::decode_square(std::string str) {
	if (str.length() == 2) {
		char colChar = str[0];
		char rowChar = str[1];

		if (isalpha(colChar) && isdigit(rowChar))
			return make_square(File(tolower(colChar) - 'a'), Rank(rowChar - '1'));
	}

	return SQ_NONE;
}

inline Move CLI::decode_move(std::string str) {
	if (str.length() == 4) {
		Square from = decode_square(str.substr(0, 2));
		Square to = decode_square(str.substr(2, 2));

		if (is_ok(from) && is_ok(to))
			return make_move(from, to);
	}

	return MOVE_NULL;
}