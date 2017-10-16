#include <iostream>

#include "bitboard.h"
#include "position.h"
#include "cli.h"

int main() {

	std::cout << "Sockfish - v1.1\n" << std::endl;

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

	std::cout << std::endl;

	CLI::loop();

	return 0;
}