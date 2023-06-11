#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "square.h"

struct Board {
    struct Square squares[8][8];
};

void setup_board(struct Board* board);
void show_board(struct Board* board);
void deallocate_board(struct Board* board);

#endif
