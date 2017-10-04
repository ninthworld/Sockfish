#include "cli.h"

void CLI::init() {
	
	std::string token;
	std::cout << "Would you like to move first (Y/n)? ";
	getline(std::cin, token);

	if (token.length() < 1 || token.at(0) == 'Y' || token.at(0) == 'y') {
		std::cout << "You will move first.\n" << std::endl;
	}
	else {
		std::cout << "The computer will move first.\n" << std::endl;
	}
}

void CLI::loop() {

	while (true) {
		printBoard();

		Move move = getMove();
	}

}

void CLI::printBoard() {
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;

	std::cout << "   ---------------------  Sockfish AI\n";
	
	for (int i = 0; i < ROWS; ++i) {
		std::printf(" %d ", 8 - i);
		for (int j = 0; j < COLS; ++j) {
			Color color;
			PieceType piece;
			Bitboards::getPieceAt(j, 7 - i, &color, &piece);

			WORD bg = ((j + i*COLS) % 2 ? BACKGROUND_BLUE : 0) | 1;
			WORD fg = (color == RED ? FOREGROUND_RED : 15) | FOREGROUND_INTENSITY;
			SetConsoleTextAttribute(stdHandle, bg | fg);
			std::printf(" %c ", PieceTypeChar[piece]);
			SetConsoleTextAttribute(stdHandle, wOldColorAttrs);
		}
		std::cout << "\n";
	}

	std::cout << "   ---------------------  Human AI\n";
	std::cout << "    A  B  C  D  E  F  G \n" << std::endl;
}

Move CLI::getMove() {

	std::string token;
	std::cout << "Please enter your move: ";
	getline(std::cin, token);

	Move move;
	if (!parseMove(token, &move)) {
		std::cout << "Invalid input. (Use format 'A1B2')" << std::endl;
		return getMove();
	}

	if (!true) { // Check Valid Move
		std::cout << "Invalid move." << std::endl;
		return getMove();
	}

	return move;
}

bool CLI::parseMove(std::string str, Move *move) {
	if (str.length() >= 4) {
		std::vector<char> token(str.c_str(), str.c_str() + str.size() + 1);

		if (isalpha(token[0])
			&& isdigit(token[1])
			&& isalpha(token[2])
			&& isdigit(token[3])) {
			token[0] = tolower(token[0]);
			token[2] = tolower(token[2]);
			if (token[0] >= 'a' && token[0] <= 'g'
				&& token[2] >= 'a' && token[2] <= 'g'
				&& token[1] >= '1' && token[1] <= '8'
				&& token[3] >= '1' && token[3] <= '8') {
				int fromCol = token[0] - 'a';
				int fromRow = token[1] - '1';
				int toCol = token[2] - 'a';
				int toRow = token[3] - '1';
				move->from = fromRow * COLS + fromCol;
				move->to = toRow * COLS + toCol;
				return true;
			}
		}
	}

	return false;
}