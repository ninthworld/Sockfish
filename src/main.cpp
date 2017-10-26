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
	
	bool showDebug = false;
	int threadCount = 6;
	int ttSizeMB = 32;

	std::string token;
	int num;
	for (int i = 0; i < argc; ++i) {
		token = std::string(argv[i]);

		if (token == "-d") {
			showDebug = true;
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

	std::cout << "Sockfish - v2.5\n" << std::endl;

	ValueMap::init();

	std::cout << "Initializing Interface......... ";
	CLI::init(showDebug);
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

	Search::clear();

	std::cout << std::endl;

	do {
		CLI::loop();

		if (!CLI::promptYesNo("Would you like to play again"))
			break;
	} while (true);

	Threads.exit();

	return 0;
}