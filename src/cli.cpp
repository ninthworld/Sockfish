#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "position.h"
#include "thread.h"
#include "cli.h"
#include "movegen.h"
#include "search.h"
#include "tt.h"

namespace CLI {

bool NullMove;
bool Debug;
std::vector<Move> MoveHistory;

} // namespace CLI

void CLI::loop() {

	MoveHistory.clear();

	Position pos;
	std::string token;
	StateListPtr states(new std::deque<StateInfo>(1));
	auto cliThread = std::make_shared<Thread>(0);
	
	Color firstTurn = RED;
	if (promptYesNo("Would you like to move first"))
		firstTurn = WHITE;

	pos.set_starting(firstTurn, RED, &states->back(), cliThread.get());
	
	while (true) {

		printPosition(pos);

		Move move;
		if (pos.side_to_move() == RED) {

			std::cout << "Computer thinking...\n" << std::endl;

			TimePoint startTime = now();
			Threads.start_thinking(pos, states, startTime);

			while (!Threads.stop) {
				if (max_time(startTime))
					Threads.stop = true;
			};

			move = Threads.best_thread()->rootMoves[0].pv;

			std::cout << "\nMove found! (" << float(now() - startTime) << "ms, depth=" << Threads.best_thread()->completedDepth << ", pv=" << encode_move(move) << ")" << std::endl;

			Threads.main()->wait_for_search_finished();

			Search::clear();
		}
		else {

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
		}

		StateInfo st;
		pos.do_move(move, st);

		std::cout << "\n" << (~pos.side_to_move() == RED ? "Computer" : "Player") << " moved " << encode_move(move);

		if (~pos.side_to_move() == RED)
			std::cout << "(" << encode_move(invert(move)) << ")";

		if (st.attackedPiece != NO_PIECE)
			std::cout << " HiYa!!";
		std::cout << std::endl;

		MoveHistory.push_back(move);

		std::cout << "\n[History] ";
		for (int i = 0; i < MoveHistory.size(); ++i) {
			if (!(i & 1))
				std::cout << (i >> 1) + 1 << ": ";
			std::cout << encode_move(MoveHistory[i]) << " ";
		}
		std::cout << std::endl;

		Color winner;
		if (pos.is_win(winner)) {
			printPosition(pos);
			std::cout << "Game Over! " << (winner == RED ? "Computer" : "Player") << " Wins!\n" << std::endl;
			break;
		}
	}
}

bool CLI::promptYesNo(std::string str) {
	std::string token;

	std::cout << str << " (Y/n)? ";
	std::getline(std::cin, token);

	return (token == "Y" || token == "y" || token == "Yes" || token == "yes" || token == "");
}

#if defined(_MSC_VER)

#include <windows.h>

void CLI::printPosition(Position &pos) {

	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;

	std::cout << "\n" << "Turn #" << ((pos.game_ply() >> 1) + 1) << " - " << (pos.side_to_move() == RED ? "Computer's" : "Player's") << " move\n\n";

	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n";
	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << "Computer" << "\n";

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

	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << "Player" << "\n";
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

#else

#define RESET           0
#define BRIGHT          1
#define BLACK           0
#define RED             1
#define BLUE            4
#define WHITE           7

void textcolor(int attr, int fg, int bg) {
	char command[13];
	sprintf_s(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

void CLI::printPosition(Position &pos) {
	
	std::cout << "\n" << "Turn #" << ((pos.game_ply() >> 1) + 1) << " - " << (pos.side_to_move() == RED ? "Computer's" : "Player's") << " move\n\n";

	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n";
	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << "Computer" << "\n";

	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		std::cout << " " << rank + 1 << " " << "|";

		for (File file = FILE_A; file <= FILE_G; ++file) {
			Piece p = pos.piece_on(make_square(file, rank));

			int bg = ((file + (int)rank * FILE_NB) % 2 ? BLUE : BLACK);
			int fg = (color_of(p) ? RED : WHITE);

			textcolor(BRIGHT, fg, bg);
			std::cout << " " << (" jsJSK")[type_of(p)] << " ";
			textcolor(RESET, WHITE, BLACK);
		}

		std::cout << "|" << " " << rank << "\n";
	}

	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "   " << "Player" << "\n";
	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n" << std::endl;

}

void CLI::printBitboard(Bitboard bb) {
	
	std::cout << "\n";
	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n";
	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "\n";

	for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
		std::cout << " " << rank + 1 << " " << "|";

		for (File file = FILE_A; file <= FILE_G; ++file) {
			Bitboard squareBB = square_bb(make_square(file, rank));

			int bg = ((file + (int)rank * FILE_NB) % 2 ? BLUE : BLACK);
			int fg = WHITE;

			textcolor(BRIGHT, fg, bg);
			std::cout << " " << (" X")[((squareBB & bb) > 0 ? 1 : 0)] << " ";
			textcolor(RESET, WHITE, BLACK);
		}

		std::cout << "|" << " " << rank << "\n";
	}

	std::cout << "   " << "+" << "---" << "---" << "---" << "---" << "---" << "---" << "---" << "+" << "\n";
	std::cout << "   " << " " << " A " << " B " << " C " << " D " << " E " << " F " << " G " << "\n" << std::endl;
}

#endif