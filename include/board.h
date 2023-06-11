#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "square.h"

typedef struct Board {
    Square squares[8][8];
} Board;

void setup_board(Board* board);
void show_board(Board* board);
void deallocate_board(Board* board);

#endif
