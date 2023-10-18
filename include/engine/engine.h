#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "board.h"
#include <vector>

Move get_best_move(int depth, const Board& board);
int perft(int depth, Board board);
void divide(int depth, Board board);

#endif
