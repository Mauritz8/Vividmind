#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "board.h"
#include "move.h"
#include "piece.h"
#include <vector>

std::vector<Move> get_legal_moves(Board& board);
bool is_in_check(Color color, Board& board);

#endif
