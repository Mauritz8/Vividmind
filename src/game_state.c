#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "move.h"
#include "piece.h"

static Square* get_king_square(const Color color, Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Piece* piece = board->squares[i][j].piece;
            if (piece && piece->piece_type == KING && piece->color == color) {
                return &board->squares[i][j];
            }
        }
    }
    return NULL;
}

static MoveArray get_legal_moves(Square* square, Board* board) {
    MoveArray legal_moves;
    int capacity = 8;
    legal_moves.moves = malloc(capacity * sizeof(Move));
    legal_moves.length = 0;

    Move move;
    move.start_square = square;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.end_square = &board->squares[i][j];
            if (validate_move_basic(&move, board)) {
                if (legal_moves.length == capacity) {
                    capacity *= 2;
                    legal_moves.moves = realloc(legal_moves.moves, capacity * sizeof(Move));
                }
                legal_moves.moves[legal_moves.length++] = move;
            }
        }
    }
    return legal_moves;
}

MoveArray get_all_legal_moves(const Color color, Board* board) {
    MoveArray all_legal_moves;
    int capacity = 16;
    all_legal_moves.moves = malloc(capacity * sizeof(Move));
    all_legal_moves.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square* square = &board->squares[i][j];
            if (square->piece && square->piece->color == color) {
                const MoveArray legal_moves = get_legal_moves(square, board);
                for (int i = 0; i < legal_moves.length; i++) {
                    if (all_legal_moves.length == capacity) {
                        capacity *= 2;
                        all_legal_moves.moves = realloc(all_legal_moves.moves, capacity * sizeof(Move));
                    }
                    all_legal_moves.moves[all_legal_moves.length++] = legal_moves.moves[i];
                }
                free(legal_moves.moves);
            }
        }
    }
    return all_legal_moves;
}

bool is_in_check(const Color color, Board* board) {
    const Square* king_square = get_king_square(color, board);

    const Color opponent_color = color == WHITE ? BLACK : WHITE;
    const MoveArray opponent_moves = get_all_legal_moves(opponent_color, board);
    for (int i = 0; i < opponent_moves.length; i++) {
        if (opponent_moves.moves[i].end_square == king_square) {
            free(opponent_moves.moves);
            return true;
        }
    }
    free(opponent_moves.moves);
    return false;
}

PieceArray get_all_pieces(const Color color, Board* board) {
    PieceArray pieces;
    int capacity = 16;
    pieces.pieces = malloc(capacity * sizeof(Piece));
    pieces.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->squares[i][j].piece;
            if (piece && piece->color == color) {
                if (pieces.length == capacity) {
                    capacity *= 2;
                    pieces.pieces = realloc(pieces.pieces, capacity * sizeof(Piece));
                }
                pieces.pieces[pieces.length++] = *piece;
            }
        }
    }
    return pieces;
}
