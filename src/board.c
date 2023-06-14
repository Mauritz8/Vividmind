#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "piece.h"

static void setup_pieces(Board* board, int row, const Piece_type* order, Color color) {
    for (int i = 0; i < 8; i++) {
        board->squares[row][i].piece = malloc(sizeof(Piece));
        board->squares[row][i].piece->piece_type = order[i];
        board->squares[row][i].piece->color = color;
    }
}

static void setup_pawns(Board* board, int row, Color color) {
    for (int i = 0; i < 8; i++) {
        board->squares[row][i].piece = malloc(sizeof(Piece));
        board->squares[row][i].piece->piece_type = PAWN;
        board->squares[row][i].piece->color = color;
    }
}

void setup_board(Board* board) {
    // setup square coordinates
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->squares[i][j].x = j;
            board->squares[i][j].y = i;
        }
    }

    // setup pieces
    const Piece_type PIECE_ORDER[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    setup_pieces(board, 0, PIECE_ORDER, BLACK);
    setup_pawns(board, 1, BLACK);
    setup_pieces(board, 7, PIECE_ORDER, WHITE);
    setup_pawns(board, 6, WHITE);
    
    // set the piece as null for all squares that don't have a piece
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            board->squares[i][j].piece = NULL;
        }
    }
}

void show_board(Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->squares[i][j].piece;
            if (piece) {
                printf(" %c", get_char_representation(piece));
            } else {
                printf(" _");
            }
        }
        printf("\n");
    }
    printf("\n");
}


void deallocate_board(Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->squares[i][j].piece; 
            if (piece) {
                free(piece);
            }
        }
    }
}

Board copy_board(Board* board) {
    Board board_copy;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square square = {square.x = j, square.y = i};
            Piece* piece = board->squares[i][j].piece;
            if (piece) {
                square.piece = malloc(sizeof(Piece));
                *square.piece = *board->squares[i][j].piece;
            } else {
                square.piece = NULL;
            }
            board_copy.squares[i][j] = square;
        }
    }
    return board_copy;
}
