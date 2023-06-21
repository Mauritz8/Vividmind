#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "square.h"

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

static SquareArray get_threatened_squares(Square* square, Board* board) {
    SquareArray threatened_squares;
    int capacity = 8;
    threatened_squares.squares = malloc(capacity * sizeof(Square));
    threatened_squares.length = 0;

    Move move;
    move.start_square = square;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.end_square = &board->squares[i][j];
            if (validate_threatened_move(&move, board)) {
                if (threatened_squares.length == capacity) {
                    capacity *= 2;
                    threatened_squares.squares = realloc(threatened_squares.squares, capacity * sizeof(Square));
                }
                threatened_squares.squares[threatened_squares.length++] = move.end_square;
            }
        }
    }
    return threatened_squares;
}

SquareArray get_all_threatened_squares(const Color color, Board* board) {
    SquareArray all_threatened_squares;
    int capacity = 16;
    all_threatened_squares.squares = malloc(capacity * sizeof(Square));
    all_threatened_squares.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square* square = &board->squares[i][j];
            if (square->piece && square->piece->color == color) {
                const SquareArray threatened_squares = get_threatened_squares(square, board);
                for (int i = 0; i < threatened_squares.length; i++) {
                    if (all_threatened_squares.length == capacity) {
                        capacity *= 2;
                        all_threatened_squares.squares = realloc(all_threatened_squares.squares, capacity * sizeof(Square));
                    }
                    all_threatened_squares.squares[all_threatened_squares.length++] = threatened_squares.squares[i];
                }
                free(threatened_squares.squares);
            }
        }
    }
    return all_threatened_squares;
}

static MoveArray get_legal_moves(Square* square, Board* board, const MoveArray* move_history) {
    MoveArray legal_moves;
    int capacity = 8;
    legal_moves.moves = malloc(capacity * sizeof(Move));
    legal_moves.length = 0;

    Move move;
    move.start_square = square;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.end_square = &board->squares[i][j];
            if (is_legal_move(&move, board, move_history)) {
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

MoveArray get_all_legal_moves(const Color color, Board* board, const MoveArray* move_history) {
    MoveArray all_legal_moves;
    int capacity = 16;
    all_legal_moves.moves = malloc(capacity * sizeof(Move));
    all_legal_moves.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square* square = &board->squares[i][j];
            if (square->piece && square->piece->color == color) {
                const MoveArray legal_moves = get_legal_moves(square, board, move_history);
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
    const SquareArray opponent_threatened_squares = get_all_threatened_squares(opponent_color, board);
    for (int i = 0; i < opponent_threatened_squares.length; i++) {
        Square* threatened_square = opponent_threatened_squares.squares[i];
        if (threatened_square == king_square) {
            free(opponent_threatened_squares.squares);
            return true;
        }
    }
    free(opponent_threatened_squares.squares);
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