#include <stdlib.h>

#include "move.h"

void make_move(struct Move move) {
    if (move.end_square->piece) {
        free(move.end_square->piece);
    }
    move.end_square->piece = move.start_square->piece;
    move.start_square->piece = NULL;
}
