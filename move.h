#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "square.h"
#include "board.h"

struct Move {
    struct Square* start_square;
    struct Square* end_square;
};

void make_move(struct Move* move);
bool is_same_line(struct Square* square1, struct Square* square2);
bool is_same_diagonal(struct Square* square1, struct Square* square2);
bool is_clear_line(struct Square* square1, struct Square* square2, struct Board* board);
bool is_clear_diagonal(struct Square* square1, struct Square* square2, struct Board* board);
bool is_valid_rook_move(struct Move* move, struct Board* board);
bool is_valid_bishop_move(struct Move* move, struct Board* board);
bool is_valid_queen_move(struct Move* move, struct Board* board);
bool is_valid_knight_move(struct Move* move);
bool is_valid_king_move(struct Move* move);
bool is_valid_pawn_move(struct Move* move, struct Board* board);

#endif
