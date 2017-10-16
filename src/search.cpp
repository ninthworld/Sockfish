#include "movegen.h"
#include "position.h"
#include "search.h"

namespace {

Value search(Position &pos, Value alpha, Value beta, Depth depth);

} // namespace

Move Search::best_move(Position &rootPos) {

	Depth rootDepth = DEPTH_ZERO;
	Value bestValue, alpha, beta;

	// Iterative deepening loop
	while ((rootDepth += ONE_PLY) < 6) {

		bestValue = ::search(rootPos, alpha, beta, rootDepth);

	}
}

namespace {

Value search(Position &pos, Value alpha, Value beta, Depth depth) {
	


}

} // namespace