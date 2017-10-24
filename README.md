# Sockfish
AI for game Hi-Ya! (CSC 180 Fall 2017)

* Minimax (implemented with Negamax)
* Alpha-Beta Pruning
* Iterative Deepening
* PV Move Ordering (top-level only)
* Killer Move
* Countermove
* History Heuristic
* Transposition Table (Zobrist Hash for key)
* Multithreading (Offset starting iterative depth to fill global transposition, history, and countermove tables)
* Pondering (Seeds transposition table, capped at 5 seconds)
* Bitboards + Magic Bitboards
* Piece Value + Positional Evaluation Heuristic