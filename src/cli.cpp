#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>

#include "position.h"
#include "thread.h"
#include "cli.h"
#include "movegen.h"
#include "search.h"

namespace {
} // namespace

void CLI::init() {
}

void CLI::loop() {

	Position pos;
	std::string token;
	StateListPtr states(new std::deque<StateInfo>(1));
	auto cliThread = std::make_shared<Thread>(0);

	Color aiSide = WHITE;
	if (promptYesNo("Would you like to play as White"))
		aiSide = RED;

	Color firstTurn = aiSide;
	if (promptYesNo("Would you like to move first"))
		firstTurn = ~aiSide;

	pos.set_starting(firstTurn, aiSide, &states->back(), cliThread.get());
	
	while (true) {

		printPosition(pos);

		if (pos.side_to_move() == pos.side_ai()) {

			std::cout << "Computer thinking..." << std::endl;

			TimePoint startTime = now();
			Threads.start_thinking(pos, states, startTime);
			Threads.main()->wait_for_search_finished();

			Move move = Threads.best_thread()->rootMoves[0].pv;

			std::cout << "Move found! (" << float(now() - startTime) << "ms, depth=" << Threads.best_thread()->completedDepth << ")" << std::endl;

			StateInfo st;
			pos.do_move(move, st);

			std::cout << "\nComputer moved " << encode_move(move);
			if (st.attackedPiece != NO_PIECE)
				std::cout << " HiYa!!";
			std::cout << std::endl;

			Search::clear();
		}
		else {

			Move move;
			do {
				std::cout << "Enter move: ";
				std::getline(std::cin, token);

				move = decode_move(token);
				if (move == MOVE_NULL)
					std::cout << "Invalid input.\n";

				if (!pos.legal(move)) {
					std::cout << "Illegal move.\n";
					move = MOVE_NULL;
				}

			} while (move == MOVE_NULL);

			StateInfo st;
			pos.do_move(move, st);

			std::cout << "\nPlayer moved " << encode_move(move);
			if (st.attackedPiece != NO_PIECE)
				std::cout << " HiYa!!";
			std::cout << std::endl;
		}		

		Color winner;
		if (pos.is_win(winner)) {
			printPosition(pos);
			std::cout << "Game Over! " << (pos.side_ai() == winner ? "Computer" : "Player") << " Wins!\n" << std::endl;
			break;
		}
	}
}

void CLI::printPosition(Position &pos) {

	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;

	std::cout << "\n" << "Turn #" << ((pos.game_ply() >> 1) + 1) << " - " << (pos.side_to_move() == pos.side_ai() ? "Computer's" : "Player's") << " move\n\n";

	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n";
	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << (pos.side_ai() == RED ? "Computer" : "Player") << "\n";

	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		std::cout << " " << rank + 1 << " " << "|";

		for (File file = FILE_A; file <= FILE_G; ++file) {
			Piece p = pos.piece_on(make_square(file, rank));

			WORD bg = ((file + (int)rank * FILE_NB) % 2 ? BACKGROUND_BLUE : 0) | 1;
			WORD fg = (color_of(p) ? FOREGROUND_RED : 15) | FOREGROUND_INTENSITY;

			SetConsoleTextAttribute(stdHandle, bg | fg);
			std::cout << " " << (" jsJSK")[type_of(p)] << " ";
			SetConsoleTextAttribute(stdHandle, wOldColorAttrs);
		}

		std::cout << "|" << " " << rank << "\n";
	}

	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << (pos.side_ai() == WHITE ? "Computer" : "Player") << "\n";
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

bool CLI::promptYesNo(std::string str) {
	std::string token;

	std::cout << str << " (Y/n)? ";
	std::getline(std::cin, token);

	return (token == "Y" || token == "y" || token == "Yes" || token == "yes" || token == "");
}