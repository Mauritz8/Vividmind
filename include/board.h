#ifndef BOARD_H
#define BOARD_H

#include "square.h"

typedef struct {
    Square squares[8][8];
} Board;

void setup_board(Board* board);
void show_board(Board* board);
void deallocate_board(Board* board);
Board copy_board(Board* board);

#endif
