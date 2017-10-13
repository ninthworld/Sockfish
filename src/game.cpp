#include "game.h"

Game::Game(PieceColor first) {
	board = Bitboards::createBoard();
	board.turn = first;
	board.ply = 0;
}

Board Game::getBoard() {
	return board;
}

void Game::setBoard(Board b) {
	board = b;
}

int Game::getTurn() {
	return (int)floor(board.ply / 2) + 1;
}