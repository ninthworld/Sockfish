#include "ai.h"

Move AI::getBestMove(Board board) {

	std::vector<Pos> piecePositions = Bitboards::getBoardColorPositions(board, RED);

	int maxScore = INT_MIN;
	Move bestMove;
	for (Pos piecePos : piecePositions) {
		std::vector<Move> moves = Moves::getLegalMoves(board, piecePos, RED, nullptr);
		for (Move move : moves) {
			Board newBoard = Moves::doMove(board, move, nullptr);

			int score = minimax(newBoard, RED, 4);
			if (score > maxScore) {
				maxScore = score;
				bestMove = move;
			}
		}
	}

	return bestMove;
}

int AI::minimax(Board board, PieceColor aiColor, int depth) {
	if (depth <= 0 || Bitboards::isBoardGameOver(board, nullptr)) {
		return getBoardScore(board);
	}

	std::vector<Pos> piecePositions = Bitboards::getBoardColorPositions(board, board.turn);

	if (board.turn == aiColor) {
		// Maximizing player

		int bestScore = INT_MIN;
		for (Pos piecePos : piecePositions) {
			std::vector<Move> moves = Moves::getLegalMoves(board, piecePos, board.turn, nullptr);
			for (Move move : moves) {
				Board newBoard = Moves::doMove(board, move, nullptr);
				int score = minimax(newBoard, aiColor, depth - 1);
				bestScore = std::max(bestScore, score);
			}
		}

		return bestScore;
	}
	else {
		// Minimizing player

		int bestScore = INT_MAX;
		for (Pos piecePos : piecePositions) {
			std::vector<Move> moves = Moves::getLegalMoves(board, piecePos, board.turn, nullptr);
			for (Move move : moves) {
				Board newBoard = Moves::doMove(board, move, nullptr);
				int score = minimax(newBoard, aiColor, depth - 1);
				bestScore = std::min(bestScore, score);
			}
		}

		return bestScore;
	}
}

int AI::getBoardScore(Board board) {
	return
		board.pieceCount[WHITE][MINI_NINJA]		* W_MINI_NINJA_SCORE +
		board.pieceCount[WHITE][MINI_SAMURAI]	* W_MINI_SAMURAI_SCORE +
		board.pieceCount[WHITE][NINJA]			* W_NINJA_SCORE +
		board.pieceCount[WHITE][SAMURAI]		* W_SAMURAI_SCORE +
		board.pieceCount[WHITE][KING]			* W_KING_SCORE +
		board.pieceCount[RED][MINI_NINJA]		* R_MINI_NINJA_SCORE +
		board.pieceCount[RED][MINI_SAMURAI]		* R_MINI_SAMURAI_SCORE +
		board.pieceCount[RED][NINJA]			* R_NINJA_SCORE +
		board.pieceCount[RED][SAMURAI]			* R_SAMURAI_SCORE +
		board.pieceCount[RED][KING]				* R_KING_SCORE;
}