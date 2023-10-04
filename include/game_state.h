#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "board.h"
#include "move.h"
#include "piece.h"
#include <functional>
#include <memory>
#include <vector>

std::vector<Move> get_all_legal_moves(const Board& board, const std::vector<Move>& move_history);
Color get_opposite_color(Color color);
bool is_check(const Board& board);
std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>> get_all_pieces(Color color, const Board& board);

#endif
