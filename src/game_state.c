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
    threatened_squares.capacity = 8;
    threatened_squares.squares = malloc(threatened_squares.capacity * sizeof(Square));
    threatened_squares.length = 0;

    Move move;
    move.start_square = square;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.end_square = &board->squares[i][j];
            if (validate_threatened_move(&move, board)) {
                square_array_push(&threatened_squares, move.end_square);
            }
        }
    }
    return threatened_squares;
}

SquareArray get_all_threatened_squares(const Color color, Board* board) {
    SquareArray all_threatened_squares;
    all_threatened_squares.capacity = 16;
    all_threatened_squares.squares = malloc(all_threatened_squares.capacity * sizeof(Square));
    all_threatened_squares.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square* square = &board->squares[i][j];
            if (square->piece && square->piece->color == color) {
                const SquareArray threatened_squares = get_threatened_squares(square, board);
                for (int i = 0; i < threatened_squares.length; i++) {
                    square_array_push(&all_threatened_squares, threatened_squares.squares[i]);
                }
                free(threatened_squares.squares);
            }
        }
    }
    return all_threatened_squares;
}

static MoveArray get_legal_moves(Square* square, Board* board, const MoveArray* move_history) {
    MoveArray legal_moves;
    legal_moves.capacity = 8;
    legal_moves.moves = malloc(legal_moves.capacity * sizeof(Move));
    legal_moves.length = 0;

    Move move;
    move.start_square = square;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.end_square = &board->squares[i][j];
            if (is_legal_move(&move, board, move_history)) {
                if (is_promotion(&move, board)) {
                    const Piece_type promotion_pieces[] = {KNIGHT, BISHOP, ROOK, QUEEN};
                    for (int i = 0; i < 4; i++) {
                        move.promotion_piece = promotion_pieces[i];
                        move_array_push(&legal_moves, &move);
                    }
                } else {
                    move_array_push(&legal_moves, &move);
                }
            }
        }
    }
    return legal_moves;
}

MoveArray get_all_legal_moves(Board* board, const MoveArray* move_history) {
    MoveArray all_legal_moves;
    all_legal_moves.capacity = 16;
    all_legal_moves.moves = malloc(all_legal_moves.capacity * sizeof(Move));
    all_legal_moves.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square* square = &board->squares[i][j];
            if (square->piece && square->piece->color == board->player_to_move) {
                const MoveArray legal_moves = get_legal_moves(square, board, move_history);
                for (int i = 0; i < legal_moves.length; i++) {
                    move_array_push(&all_legal_moves, &legal_moves.moves[i]);
                }
                free(legal_moves.moves);
            }
        }
    }
    return all_legal_moves;
}

Color get_opposite_color(const Color color) {
    return color == WHITE ? BLACK : WHITE;
}

bool is_check(Board* board) {
    const Square* king_square = get_king_square(board->player_to_move, board);

    const Color opponent_color = get_opposite_color(board->player_to_move);
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
    pieces.capacity = 16;
    pieces.pieces = malloc(pieces.capacity * sizeof(Piece));
    pieces.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->squares[i][j].piece;
            if (piece && piece->color == color) {
                piece_array_push(&pieces, piece);
            }
        }
    }
    return pieces;
}

static void deallocate_move_history(MoveArray* move_history) {
    for (int i = 0; i < move_history->length; i++) {
        Move move = move_history->moves[i];
        free(move.start_square);
        free(move.end_square);
        free(move.captured_piece);
    }
    free(move_history->moves);
}

void deallocate_game_resources(Board* board, MoveArray* move_history) {
    deallocate_board(board);
    deallocate_move_history(move_history);
}

MoveArray create_empty_move_history() {
    MoveArray move_history;
    move_history.capacity = 16;
    move_history.moves = malloc(move_history.capacity * sizeof(Move));
    move_history.length = 0;
    return move_history;
}
