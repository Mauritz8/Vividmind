#ifndef BOARD_H
#define BOARD_H

#include "square.h"

typedef struct {
    Square squares[8][8];
    Color player_to_move;
} Board;

void setup_board(Board* board);
void show_board(const Board* board);
void deallocate_board(Board* board);
Board copy_board(const Board* board);

#endif
