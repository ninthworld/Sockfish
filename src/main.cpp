#include <iostream>
#include "cli.h"

int main() {

	std::cout << "Sockfish AI - v0.1\n" << std::endl;

	CLI::init();
	Bitboards::init();

	CLI::loop();

	return 0;
}