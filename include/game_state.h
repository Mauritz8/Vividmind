#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "board.h"
#include "move.h"
#include "piece.h"
#include <functional>
#include <memory>
#include <vector>

std::vector<Move> get_legal_moves(const Board& board);
Color get_opposite_color(Color color);
bool is_in_check(Color color, const Board& board);
std::vector<std::shared_ptr<Piece>> get_all_pieces(Color color, const Board& board);

#endif
