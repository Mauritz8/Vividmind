#include <stdlib.h>

#include "test_utils.h"
#include "move.h"

void setup_empty_board(Board* board) {
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

void place_piece_at(const Piece* piece, Board* board, const int x, const int y) {
    board->squares[y][x].piece = malloc(sizeof(Piece));
    *board->squares[y][x].piece = *piece;
}
