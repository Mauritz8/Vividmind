#include <stdlib.h>

#include "move.h"

void make_move(struct Move* move) {
    if (move->end_square->piece) {
        free(move->end_square->piece);
    }
    move->end_square->piece = move->start_square->piece;
    move->start_square->piece = NULL;
}

bool is_same_line(struct Square* square1, struct Square* square2) {
    return square1->x == square2->x || square1->y == square2->y;
}

bool is_same_diagonal(struct Square* square1, struct Square* square2) {
    return abs(square1->x - square2->x) == abs(square1->y - square2->y);
}

bool is_clear_line(struct Square* square1, struct Square* square2, struct Board* board) {
    int x_direction = 0;
    int y_direction = 0;

    if (square1->x != square2->x) {
        x_direction = (square2->x - square1->x) > 0 ? 1 : -1;
    } else if (square1->y != square2->y) {
        y_direction = (square2->y - square1->y) > 0 ? 1 : -1;
    }

    int x = square1->x + x_direction;
    int y = square1->y + y_direction;
    while (x != square2->x || y != square2->y) {
        if (board->squares[y][x].piece) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

bool is_clear_diagonal(struct Square* square1, struct Square* square2, struct Board *board) {
    int x_direction = (square2->x - square1->x) > 0 ? 1 : -1;
    int y_direction = (square2->y - square1->y) > 0 ? 1 : -1;

    int x = square1->x + x_direction;
    int y = square1->y + y_direction;
    while (x != square2->x && y != square2->y) {
        if (board->squares[y][x].piece) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

bool is_valid_rook_move(struct Move* move, struct Board* board) {
    if (!is_same_line(move->start_square, move->end_square)) {
        return false;
    }
    if (!is_clear_line(move->start_square, move->end_square, board)) {
        return false;
    }
    return true;
}

bool is_valid_bishop_move(struct Move* move, struct Board* board) {
    if (!is_same_diagonal(move->start_square, move->end_square)) {
        return false;
    }
    if (!is_clear_diagonal(move->start_square, move->end_square, board)) {
        return false;
    }
    return true;
}

bool is_valid_queen_move(struct Move* move, struct Board* board) {
    if (is_valid_rook_move(move, board)) {
        return true;
    }
    if (is_valid_bishop_move(move, board)) {
        return true;
    }
    return false;
}

bool is_valid_knight_move(struct Move* move) {
    int start_x = move->start_square->x;
    int start_y = move->start_square->y;
    int end_x = move->end_square->x;
    int end_y = move->end_square->y;

    if (abs(end_x - start_x) == 1 && abs(end_y - start_y) == 2) {
        return true;
    }
    if (abs(end_x - start_x) == 2 && abs(end_y - start_y) == 1) {
        return true;
    }
    return false;
}

bool is_valid_king_move(struct Move* move) {
    int x_diff = abs(move->start_square->x - move->end_square->x);
    int y_diff = abs(move->start_square->y - move->end_square->y);
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

bool is_valid_pawn_move(struct Move* move, struct Board* board) {
    int direction = move->start_square->piece->color == BLACK ? 1 : -1;

    int x_diff = move->end_square->x - move->start_square->x;
    int y_diff = move->end_square->y - move->start_square->y;

    bool end_square_is_empty = move->end_square->piece == NULL;
    bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && end_square_is_empty;

    int starting_row = move->start_square->piece->color == BLACK ? 1 : 6;
    bool is_on_starting_row = move->start_square->y == starting_row;
    bool one_square_forward_is_empty = board->squares[move->start_square->y + direction][move->start_square->x].piece == NULL;
    bool is_valid_move_two_squares_forward = x_diff == 0 && y_diff == 2 * direction && is_on_starting_row && one_square_forward_is_empty && end_square_is_empty;

    bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square_is_empty && move->end_square->piece->color != move->start_square->piece->color;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

bool is_square_outside_board(struct Square* square) {
    return square->x < 0 || square->x > 7 || square->y < 0 || square->y > 7;
}

bool is_valid_move(struct Move* move, struct Board* board) {
    if (is_square_outside_board(move->start_square) || is_square_outside_board(move->end_square)) {
        return false;
    }

    if (move->start_square->piece == NULL) {
        return false;
    }

    if (move->end_square->piece && move->start_square->piece->color == move->end_square->piece->color) {
        return false;
    }

    switch (move->start_square->piece->piece_type) {
        case PAWN:
            return is_valid_pawn_move(move, board);
        case KNIGHT:
            return is_valid_knight_move(move);
        case BISHOP:
            return is_valid_bishop_move(move, board);
        case ROOK:
            return is_valid_rook_move(move, board);
        case QUEEN:
            return is_valid_queen_move(move, board);
        case KING:
            return is_valid_king_move(move);
    }
}
