#include "game.h"

Game::Game(PieceColor startColor) :
	currentTurn(startColor),
	halfTurnNum(1),
	turnNum(1) {
}

PieceColor Game::getCurrentTurn() {
	return currentTurn;
}

unsigned int Game::getTurnNum() {
	return turnNum;
}

void Game::nextTurn() {
	currentTurn = (currentTurn == WHITE ? RED : WHITE);
	halfTurnNum++;
	if (halfTurnNum % 2) {
		turnNum++;
	}
}