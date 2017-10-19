#include <iostream>

#include "bitboard.h"
#include "position.h"
#include "thread.h"
#include "cli.h"

int main() {

	std::cout << "Sockfish - v1.2\n" << std::endl;

	ValueMap::init();

	std::cout << "Initializing Interface....... ";
	CLI::init();
	std::cout << "Done\n";

	std::cout << "Initializing Bitboards....... ";
	Bitboards::initBBs();
	std::cout << "Done\n";

	std::cout << "Generating Magic Bitboards... ";
	Bitboards::initMagicBBs();
	std::cout << "Done\n";

	std::cout << "Initializing Positions....... ";
	Position::init();
	std::cout << "Done\n";

	std::cout << "Initializing Threads......... ";
	Threads.init(MAX_THREADS);
	std::cout << "Done\n";

	Search::clear();

	std::cout << std::endl;

	std::string token;
	do {
		
		CLI::loop();

		if (!CLI::promptYesNo("Would you like to play again"))
			break;
	} while (true);

	Threads.exit();

	return 0;
}