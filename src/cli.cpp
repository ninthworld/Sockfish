#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>

#include "position.h"
#include "cli.h"
#include "movegen.h"

namespace {
} // namespace

void CLI::init() {
}

void CLI::loop() {

	Position pos;
	std::string token;

	std::cout << "Would you like to move first (y/n)? ";
	std::getline(std::cin, token);

	Color firstTurn = RED;
	if (token == "Y" || token == "y" || token == "Yes" || token == "yes") {
		firstTurn = WHITE;
	}

	pos.set_starting(firstTurn);

	while (true) {

		printPosition(pos);

		if (pos.side_to_move() == WHITE) {

			Move move;
			do {
				std::cout << "Enter move: ";
				std::getline(std::cin, token);

				move = decode_move(token);
				if (move == MOVE_NULL)
					std::cout << "Invalid input.\n";

				if (false) { // Illegal Move
					std::cout << "Illegal move.\n";
					move = MOVE_NULL;
				}

			} while (move == MOVE_NULL);

			// Do move


			/*
			// DEBUG
			Square s = from_sq(move);
			Piece p = pos.piece_on(s);
			Bitboard legal = moves_bb(s, p, pos.pieces()) | attacks_bb(s, p, pos.pieces(WHITE), pos.pieces(RED));
			// DEBUG

			printBitboard(legal);

			ExtMove moves[MAX_MOVES];
			ExtMove *cur = moves;
			ExtMove *endMoves = generate(pos, cur);

			while (cur < endMoves) {
				std::cout << encode_move(cur++->move) << ", ";
			}

			std::cout << std::endl;
			*/

			Bitboard b = pos.moves(SQ_E2);
			std::cout << encode_move(make_move(SQ_E2, pop_lsb(&b))) << std::endl;
			std::cout << encode_move(make_move(SQ_E2, pop_lsb(&b))) << std::endl;
			std::cout << encode_move(make_move(SQ_E2, pop_lsb(&b))) << std::endl;

		}
		else {

			// AI Logic
			std::getline(std::cin, token);

		}

		// Check for winner

	}
}

void CLI::printPosition(Position &pos) {

	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;

	std::cout << "\n" << "Turn #" << ((pos.game_ply() >> 1) + 1) << "\n\n";

	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n";
	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << "Computer" << "\n";

	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		std::cout << " " << rank + 1 << " " << "|";

		for (File file = FILE_A; file <= FILE_G; ++file) {
			Piece p = pos.piece_on(make_square(file, rank));

			WORD bg = ((file + (int)rank * FILE_NB) % 2 ? BACKGROUND_BLUE : 0) | 1;
			WORD fg = (color_of(p) ? FOREGROUND_RED : 15) | FOREGROUND_INTENSITY;

			SetConsoleTextAttribute(stdHandle, bg | fg);
			std::cout << " " << (" nsNSK")[type_of(p)] << " ";
			SetConsoleTextAttribute(stdHandle, wOldColorAttrs);
		}

		std::cout << "|" << " " << rank << "\n";
	}

	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << "Human" << "\n";
	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n" << std::endl;

}

void CLI::printBitboard(Bitboard bb) {

	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;
	
	std::cout << "\n";
	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n";
	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "\n";

	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		std::cout << " " << rank + 1 << " " << "|";

		for (File file = FILE_A; file <= FILE_G; ++file) {
			WORD bg = ((file + (int)rank * FILE_NB) % 2 ? BACKGROUND_BLUE : 0) | 1;
			WORD fg = 15 | FOREGROUND_INTENSITY;

			Bitboard squareBB = square_bb(make_square(file, rank));

			SetConsoleTextAttribute(stdHandle, bg | fg);
			std::cout << " " << (" X")[((squareBB & bb) > 0 ? 1 : 0)] << " ";
			SetConsoleTextAttribute(stdHandle, wOldColorAttrs);
		}

		std::cout << "|" << " " << rank << "\n";
	}

	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "\n";
	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n" << std::endl;
}