#include <iostream>
#include "cli.h"
#include "game.h"

int main() {

	std::cout << "Sockfish AI - v0.1\n" << std::endl;

	PieceColor startColor = CLI::init();
	Game game = Game(startColor);

	Bitboards::init();
	Moves::init();

	CLI::loop(&game);

	return 0;
}