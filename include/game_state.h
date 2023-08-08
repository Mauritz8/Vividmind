#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "board.h"
#include "move.h"
#include "piece.h"

SquareArray get_all_threatened_squares(const Color color, Board* board);
MoveArray get_all_legal_moves(Board* board, const MoveArray* move_history);
Color get_opposite_color(const Color color);
bool is_check(Board* board);
PieceArray get_all_pieces(const Color color, Board* board);
void deallocate_game_resources(Board* board, MoveArray* move_history);
MoveArray create_empty_move_history();
void switch_player_to_move(Board* board);

#endif
