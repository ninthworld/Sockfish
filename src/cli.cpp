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

	PieceColor winner = NO_PIECE_COLOR;
	while (!Bitboards::isBoardGameOver(game->getBoard(), &winner)) {
		printBoard(game);

		if (game->getBoard().turn == WHITE) {
			// Player Move

			Move move = getMove(game);
			
			std::cout << "Player moved " << encodeMove(move);

			bool hiya = false;
			Board newBoard = Moves::doMove(game->getBoard(), move, &hiya);
			game->setBoard(newBoard);

			if (hiya) {
				std::cout << " HiYA!" << std::endl;
			}
			else {
				std::cout << std::endl;
			}
		}
		else {
			// AI Move

			Move move = AI::getBestMove(game->getBoard());

			std::cout << "AI moved " << encodeMove(move);

			bool hiya = false;
			Board newBoard = Moves::doMove(game->getBoard(), move, &hiya);
			game->setBoard(newBoard);

			if (hiya) {
				std::cout << " HiYA!" << std::endl;
			}
			else {
				std::cout << std::endl;
			}
		}
	}

	if (winner == WHITE) {
		std::cout << "White Wins!" << std::endl;
	}
	else if (winner == RED) {
		std::cout << "Red Wins!" << std::endl;
	}

	std::string token;
	std::cout << "Would you like to play again (Y/n)? ";
	getline(std::cin, token);

	if (token.length() < 1 || token.at(0) == 'Y' || token.at(0) == 'y') {
		Game game = Game(init());
		CLI::loop(&game);
	}
}

Move CLI::getMove(Game *game) {

	std::string token;
	std::cout << "Please enter your move: ";
	getline(std::cin, token);

	Move move;
	try {
		move = decodeMove(token);
	}
	catch (std::exception e) {
		std::cout << "Invalid input. (Use format 'A1B2')" << std::endl;
		return getMove(game);
	}

	/*
	Bitboard legalMovesBB = 0x0ULL;
	std::vector<Move> moves = Moves::getLegalMoves(move.from, &legalMovesBB);
	printBitboard(moves, legalMovesBB);
	*/

	if (!Moves::isMoveLegal(game->getBoard(), move, game->getBoard().turn)) {
		std::cout << "Invalid move." << std::endl;
		return getMove(game);
	}

	return move;
}

void CLI::printBoard(Game *game) {
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;
	
	std::cout << "\nTurn #" << game->getTurn() << "\n";

	std::cout << "   --------------------- " << (game->getBoard().turn == RED ? "(turn)" : "      ") << " " << AI_NAME << "\n";
	
	for (int i = 0; i < ROWS; ++i) {
		std::printf(" %d ", 8 - i);
		for (int j = 0; j < COLS; ++j) {
			Piece piece = Bitboards::getPieceAt(game->getBoard(), { 7 - i, j });

			WORD bg = ((j + i*COLS) % 2 ? BACKGROUND_BLUE : 0) | 1;
			WORD fg = (piece.color == RED ? FOREGROUND_RED : 15) | FOREGROUND_INTENSITY;
			SetConsoleTextAttribute(stdHandle, bg | fg);
			std::printf(" %c ", PieceTypeChar[piece.type]);
			SetConsoleTextAttribute(stdHandle, wOldColorAttrs);
		}
		std::cout << "\n";
	}

	std::cout << "   --------------------- " << (game->getBoard().turn == WHITE ? "(turn)" : "      ") << " Human\n";
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