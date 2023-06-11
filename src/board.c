#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "piece.h"

static void setup_pieces(Board* board, int row, const enum Piece_type* order, enum Color color) {
    for (int i = 0; i < 8; i++) {
        board->squares[row][i].piece = malloc(sizeof(Piece));
        board->squares[row][i].piece->piece_type = order[i];
        board->squares[row][i].piece->color = color;
    }
}

static void setup_pawns(Board* board, int row, enum Color color) {
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
    const enum Piece_type PIECE_ORDER[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
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
