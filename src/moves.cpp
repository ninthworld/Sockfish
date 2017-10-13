#include "moves.h"

Bitboard Moves::genMovesBB[PIECE_COLOR_NB][PIECE_TYPE_NB][ROWS][COLS][MOVE_DIRECTION_NB][MOVE_TYPE_NB][MAX_STEPS];

void Moves::init() {
	for (unsigned int color = WHITE; color < PIECE_COLOR_NB; ++color) {
		for (unsigned int type = MINI_NINJA; type < PIECE_TYPE_NB; ++type) {
			for (int row = 0; row < ROWS; ++row) {
				for (int col = 0; col < COLS; ++col) {
					Pos pos = { row, col };

					for (unsigned int dir = 0; dir < MOVE_DIRECTION_NB; ++dir) {
						Pos deltaDiagonal = { (dir % 2 ? 1 : -1), (dir < 2 ? 1 : -1) };
						Pos deltaStraight = { (dir < 2 ? (dir < 1 ? -1 : 1) : 0), (dir >= 2 ? (dir < 3 ? -1 : 1) : 0) };

						for (int steps = 0; steps < MAX_STEPS; ++steps) {
							if (type == MINI_NINJA || type == NINJA) {
								Pos jumpPos = { row + deltaDiagonal.row * (steps + 1), col + deltaDiagonal.col * (steps + 1) };

								if (jumpPos.row >= 0 && jumpPos.row < ROWS && jumpPos.col >= 0 && jumpPos.col < COLS) {
									Bitboard jumpBB = 0x1ULL << getPosIndex(jumpPos);
									setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_NORMAL, steps, jumpBB);

									if ((color == WHITE && deltaDiagonal.row < 0) || (color == RED && deltaDiagonal.row > 0)) {
										Bitboard bb = (color == WHITE ? (jumpBB << COLS) : (jumpBB >> COLS));
										setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_ATTACK, steps, bb);
									}
									else {
										setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_ATTACK, steps, (Bitboard)_UI64_MAX);
									}
								}
							}
							else if (type == MINI_SAMURAI || type == SAMURAI) {
								Pos jumpPos = { row + deltaStraight.row * (steps + 1), col + deltaStraight.col * (steps + 1) };

								if (jumpPos.row >= 0 && jumpPos.row < ROWS && jumpPos.col >= 0 && jumpPos.col < COLS) {
									Bitboard jumpBB = 0x1ULL << getPosIndex(jumpPos);
									setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_NORMAL, steps, jumpBB);

									if (deltaStraight.col != 0) {
										Bitboard bb = (color == WHITE ? (jumpBB << COLS) : (jumpBB >> COLS));
										setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_ATTACK, steps, bb);
									}
									else if ((color == WHITE && deltaStraight.row < 0) || (color == RED && deltaStraight.row > 0)) {
										setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_ATTACK, steps, (Bitboard)0x0ULL);
									}
									else {
										setGenMoves((PieceColor)color, (PieceType)type, pos, (MoveDirection)dir, MOVE_ATTACK, steps, (Bitboard)_UI64_MAX);
									}

								}
							}
						}
					}
				}
			}
		}
	}
}

Bitboard Moves::getGenMoves(PieceColor color, PieceType pieceType, Pos pos, MoveDirection dir, MoveType moveType, unsigned int step) {
	return genMovesBB[color][pieceType][pos.row][pos.col][dir][moveType][step];
}

void Moves::setGenMoves(PieceColor color, PieceType pieceType, Pos pos, MoveDirection dir, MoveType moveType, unsigned int step, Bitboard bb) {
	genMovesBB[color][pieceType][pos.row][pos.col][dir][moveType][step] = bb;
}

std::vector<Move> Moves::getLegalMoves(Pos pos, PieceColor currentTurn, Bitboard *bb) {
	std::vector<Move> legalMoves;

	Piece piece = Bitboards::getPieceAt(pos);

	if (piece.color && piece.type && piece.color == currentTurn) {
		Bitboard whiteBB = Bitboards::getBoardColor(WHITE);
		Bitboard redBB = Bitboards::getBoardColor(RED);
		Bitboard allBBInv = ~(whiteBB | redBB);

		unsigned int maxSteps = (piece.type == MINI_NINJA || piece.type == MINI_SAMURAI ? 1 : MAX_STEPS);
		for (unsigned int dir = 0; dir < MOVE_DIRECTION_NB; ++dir) {
			for (unsigned int step = 0; step < maxSteps; ++step) {
				Bitboard moveToBB = (getGenMoves(piece.color, piece.type, pos, (MoveDirection)dir, MOVE_NORMAL, step) & allBBInv);

				if (moveToBB) {
					Bitboard attackBB = getGenMoves(piece.color, piece.type, pos, (MoveDirection)dir, MOVE_ATTACK, step);

					if ((piece.color == WHITE && (attackBB & redBB)) ||
						(piece.color == RED && (attackBB & whiteBB))) {
						legalMoves.push_back({ pos, getPos(moveToBB) });
						*bb |= moveToBB;
					}
				}
				else {
					break;
				}
			}
		}
	}

	return legalMoves;
}

bool Moves::isMoveLegal(Move move, PieceColor currentTurn) {
	Bitboard moveBB = 0x1ULL << getPosIndex(move.to);

	Bitboard legalMovesBB = 0x0ULL;
	getLegalMoves(move.from, currentTurn, &legalMovesBB);

	return (legalMovesBB & moveBB);
}

void Moves::doMove(Move move) {
	Piece piece = Bitboards::getPieceAt(move.from);

	Bitboard fromBB = 0x1ULL << getPosIndex(move.from);
	Bitboard toBB = 0x1ULL << getPosIndex(move.to);

	Bitboards::setBoard(piece, Bitboards::getBoard(piece) ^ (fromBB | toBB));

	Piece attack;
	switch (piece.color) {
	case WHITE:
		attack = Bitboards::getPieceAt({ move.to.row + 1, move.to.col });
		if (attack.color == RED) {
			Bitboard attackBB = (toBB << COLS);
			Bitboards::setBoard(attack, Bitboards::getBoard(attack) ^ attackBB);

			if (attack.type == NINJA) {
				Bitboards::setBoard({ attack.color, MINI_NINJA }, Bitboards::getBoard({ attack.color, MINI_NINJA }) ^ attackBB);
			} else if (attack.type == SAMURAI) {
				Bitboards::setBoard({ attack.color, MINI_SAMURAI }, Bitboards::getBoard({ attack.color, MINI_SAMURAI }) ^ attackBB);
			}
		}
		break;
	case RED:
		attack = Bitboards::getPieceAt({ move.to.row - 1, move.to.col });
		if (attack.color == WHITE) {
			Bitboard attackBB = (toBB >> COLS);
			Bitboards::setBoard(attack, Bitboards::getBoard(attack) ^ attackBB);

			if (attack.type == NINJA) {
				Bitboards::setBoard({ attack.color, MINI_NINJA }, Bitboards::getBoard({ attack.color, MINI_NINJA }) ^ attackBB);
			}
			else if (attack.type == SAMURAI) {
				Bitboards::setBoard({ attack.color, MINI_SAMURAI }, Bitboards::getBoard({ attack.color, MINI_SAMURAI }) ^ attackBB);
			}
		}
		break;
	}
}