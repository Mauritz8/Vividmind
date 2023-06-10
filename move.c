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

bool is_valid_queen_move(struct Move *move, struct Board *board) {
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

bool is_valid_king_move(struct Move *move) {
    int x_diff = abs(move->start_square->x - move->end_square->x);
    int y_diff = abs(move->start_square->y - move->end_square->y);
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}
