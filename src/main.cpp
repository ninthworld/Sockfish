#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "bitboard.h"
#include "position.h"
#include "thread.h"
#include "tt.h"
#include "cli.h"

int main(int argc, char *argv[]) {
	
	bool nullMove = false;
	bool showDebug = true;
	int threadCount = 6;
	int ttSizeMB = 32;

	std::string token;
	int num;
	for (int i = 0; i < argc; ++i) {
		token = std::string(argv[i]);

		if (token == "-d") {
			showDebug = true;
		}
		else if (token == "-n") {
			nullMove = true;
		}
		else if (token == "-t") {
			if (i + 1 < argc) {
				std::istringstream iss(argv[++i]);
				iss >> num;
				threadCount = std::max(1, std::min(512, num));
			}
		}
		else if (token == "-s") {
			if (i + 1 < argc) {
				std::istringstream iss(argv[++i]);
				iss >> num;
				ttSizeMB = std::max(1, std::min(512, num));
			}
		}
	}

	std::cout << "Sockfish - v2.4\n" << std::endl;

	ValueMap::init();

	std::cout << "Initializing Interface......... ";
	CLI::init(showDebug, nullMove);
	std::cout << "Done\n";

	std::cout << "Initializing Bitboards......... ";
	Bitboards::initBBs();
	std::cout << "Done\n";

	std::cout << "Initializing Magic Bitboards... ";
	Bitboards::initMagicBBs();
	std::cout << "Done\n";

	std::cout << "Initializing Positions......... ";
	Position::init();
	std::cout << "Done\n";

	std::printf( "Initializing %3dMB TT.......... ", ttSizeMB);
	TT.resize(ttSizeMB);
	std::cout << "Done\n";

	std::printf( "Initializing %2d Threads........ ", threadCount);
	Threads.init(threadCount);
	std::cout << "Done\n";

	std::cout << "Show Debug   : " << (showDebug ? "ENABLED" : "DISABLED") << std::endl;
	std::cout << "Use NullMove : " << (nullMove ? "ENABLED" : "DISABLED") << std::endl;

	Search::clear();

	std::cout << std::endl;

	do {
		std::thread cliThread(CLI::loop);
		cliThread.join();

		if (!CLI::promptYesNo("Would you like to play again"))
			break;
	} while (true);

	Threads.exit();

	return 0;
}