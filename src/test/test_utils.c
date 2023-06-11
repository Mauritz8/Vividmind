#include <stdlib.h>

#include "test/test_utils.h"

void setup_empty_board(struct Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->squares[i][j].x = j;
            board->squares[i][j].y = i;
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->squares[i][j].piece = NULL;
        }
    }
}

void place_piece_at(struct Piece* piece, struct Board* board, int x, int y) {
    board->squares[y][x].piece = malloc(sizeof(struct Piece));
    *(board->squares[y][x].piece) = *piece;
}
