#include "cli.h"

PieceColor CLI::init() {
	
	std::string token;
	std::cout << "Would you like to move first (Y/n)? ";
	getline(std::cin, token);

	if (token.length() < 1 || token.at(0) == 'Y' || token.at(0) == 'y') {
		std::cout << "You will move first.\n" << std::endl;
		return WHITE;
	}
	else {
		std::cout << "The computer will move first.\n" << std::endl;
		return RED;
	}
}

void CLI::loop(Game *game) {

	while (true) {
		printBoard(game->getCurrentTurn(), game->getTurnNum());

		if (game->getCurrentTurn() == WHITE) {
			Move move = getMove(game->getCurrentTurn());
			Moves::doMove(move);
		}

		game->nextTurn();
	}

}

Move CLI::getMove(PieceColor currentTurn) {

	std::string token;
	std::cout << "Please enter your move: ";
	getline(std::cin, token);

	Move move;
	try {
		move = decodeMove(token);
	}
	catch (std::exception e) {
		std::cout << "Invalid input. (Use format 'A1B2')" << std::endl;
		return getMove(currentTurn);
	}

	/*
	Bitboard legalMovesBB = 0x0ULL;
	std::vector<Move> moves = Moves::getLegalMoves(move.from, &legalMovesBB);
	printBitboard(moves, legalMovesBB);
	*/

	if (!Moves::isMoveLegal(move, currentTurn)) {
		std::cout << "Invalid move." << std::endl;
		return getMove(currentTurn);
	}

	return move;
}

void CLI::printBoard(PieceColor currentTurn, unsigned int turnNum) {
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;
	
	std::cout << "\nTurn #" << turnNum << "\n";

	std::cout << "   --------------------- " << (currentTurn == RED ? "(turn)" : "      ") << " Sockfish AI\n";
	
	for (int i = 0; i < ROWS; ++i) {
		std::printf(" %d ", 8 - i);
		for (int j = 0; j < COLS; ++j) {
			Piece piece = Bitboards::getPieceAt({ 7 - i, j });

			WORD bg = ((j + i*COLS) % 2 ? BACKGROUND_BLUE : 0) | 1;
			WORD fg = (piece.color == RED ? FOREGROUND_RED : 15) | FOREGROUND_INTENSITY;
			SetConsoleTextAttribute(stdHandle, bg | fg);
			std::printf(" %c ", PieceTypeChar[piece.type]);
			SetConsoleTextAttribute(stdHandle, wOldColorAttrs);
		}
		std::cout << "\n";
	}

	std::cout << "   --------------------- " << (currentTurn == WHITE ? "(turn)" : "      ") << " Human\n";
	std::cout << "    A  B  C  D  E  F  G \n" << std::endl;
}

void CLI::printBitboard(std::vector<Move> moves, Bitboard bitboard) {
	std::vector<std::string> lines;
	
	for (unsigned int row = 0; row < ROWS; ++row) {
		std::string line = "";
		for (unsigned int col = 0; col < COLS; ++col) {
			line.append(std::to_string(bitboard & 0x1ULL));
			line.append(" ");
			bitboard >>= 1;
		}
		lines.push_back(line);
	}

	for (int row = ROWS - 1; row >= 0; --row) {
		std::cout << lines[row] << "\n";
	}

	for (Move move : moves) {
		std::cout << encodeMove(move) << " ";
	}

	std::cout << std::endl;

}

std::string CLI::encodePos(Pos pos) {
	std::string str;
	str.push_back((char)pos.col + 'a');
	str.push_back((char)pos.row + '1');
	return str;
}

std::string CLI::encodeMove(Move move) {
	std::string str = encodePos(move.from);
	str.append(encodePos(move.to));
	return str;
}

Pos CLI::decodePos(std::string str) {
	if (str.length() == 2) {
		char colChar = str[0];
		char rowChar = str[1];
		if (isalpha(colChar) && isdigit(rowChar)) {
			return { rowChar - '1', tolower(colChar) - 'a' };
		}
		else {
			throw std::exception();
		}
	}
	else {
		throw std::exception();
	}
}

Move CLI::decodeMove(std::string str) {
	if (str.length() == 4) {
		Pos from = decodePos(str.substr(0, 2));
		Pos to = decodePos(str.substr(2, 2));
		return { from, to };
	}
	else {
		throw std::exception();
	}
}