#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "piece.h"
#include "game_state.h"

void make_move(const Move* move, Board* board) {
    Square* start_square = &board->squares[move->start_square->y][move->start_square->x]; 
    Square* end_square = &board->squares[move->end_square->y][move->end_square->x]; 
    if (end_square->piece) {
        free(end_square->piece);
    }
    end_square->piece = start_square->piece;
    start_square->piece = NULL;
}

static bool is_same_line(const Square* square1, const Square* square2) {
    return square1->x == square2->x || square1->y == square2->y;
}

static bool is_same_diagonal(const Square* square1, const Square* square2) {
    return abs(square1->x - square2->x) == abs(square1->y - square2->y);
}

static bool is_clear_line(const Square* square1, const Square* square2, const Board* board) {
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

static bool is_clear_diagonal(const Square* square1, const Square* square2, const Board *board) {
    const int x_direction = (square2->x - square1->x) > 0 ? 1 : -1;
    const int y_direction = (square2->y - square1->y) > 0 ? 1 : -1;

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

static bool is_valid_rook_move(const Move* move, const Board* board) {
    if (!is_same_line(move->start_square, move->end_square)) {
        return false;
    }
    if (!is_clear_line(move->start_square, move->end_square, board)) {
        return false;
    }
    return true;
}

static bool is_valid_bishop_move(const Move* move, const Board* board) {
    if (!is_same_diagonal(move->start_square, move->end_square)) {
        return false;
    }
    if (!is_clear_diagonal(move->start_square, move->end_square, board)) {
        return false;
    }
    return true;
}

static bool is_valid_queen_move(const Move* move, const Board* board) {
    if (is_valid_rook_move(move, board)) {
        return true;
    }
    if (is_valid_bishop_move(move, board)) {
        return true;
    }
    return false;
}

static bool is_valid_knight_move(const Move* move) {
    const int start_x = move->start_square->x;
    const int start_y = move->start_square->y;
    const int end_x = move->end_square->x;
    const int end_y = move->end_square->y;

    if (abs(end_x - start_x) == 1 && abs(end_y - start_y) == 2) {
        return true;
    }
    if (abs(end_x - start_x) == 2 && abs(end_y - start_y) == 1) {
        return true;
    }
    return false;
}

static bool is_valid_king_move(const Move* move) {
    const int x_diff = abs(move->start_square->x - move->end_square->x);
    const int y_diff = abs(move->start_square->y - move->end_square->y);
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

static bool is_valid_pawn_move(const Move* move, const Board* board) {
    const int direction = move->start_square->piece->color == BLACK ? 1 : -1;

    const int x_diff = move->end_square->x - move->start_square->x;
    const int y_diff = move->end_square->y - move->start_square->y;

    const bool end_square_is_empty = move->end_square->piece == NULL;
    const bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && end_square_is_empty;

    const int starting_row = move->start_square->piece->color == BLACK ? 1 : 6;
    const bool is_on_starting_row = move->start_square->y == starting_row;
    const bool one_square_forward_is_empty = board->squares[move->start_square->y + direction][move->start_square->x].piece == NULL;
    const bool is_valid_move_two_squares_forward = x_diff == 0 && y_diff == 2 * direction && is_on_starting_row && one_square_forward_is_empty && end_square_is_empty;

    const bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square_is_empty && move->end_square->piece->color != move->start_square->piece->color;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

static bool is_square_outside_board(const Square* square) {
    return square->x < 0 || square->x > 7 || square->y < 0 || square->y > 7;
}

bool validate_move_basic(const Move* move, const Board* board) {
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

bool leaves_king_in_check(const Move* move, const Board* board) {
    Board board_copy = copy_board(board);
    const Color color_to_move = move->start_square->piece->color;
    make_move(move, &board_copy);
    if (is_in_check(color_to_move, &board_copy)) {
        deallocate_board(&board_copy);
        return true;
    }
    deallocate_board(&board_copy);
    return false;
}

bool is_castling_move(const Move* move) {
    const int starting_row = move->start_square->piece->color == WHITE ? 7 : 0;
    const int king_x = 4;
    if (move->start_square->x != king_x || move->start_square->y != starting_row) {
        return false;
    }
    if (move->end_square->x != 6 && move->end_square->x != 2) {
        return false;
    }
    return true;
}

static bool has_castling_pieces_moved(const MoveArray* move_history, const int starting_row, const int king_x, const int rook_x) {
    for (int i = 0; i < move_history->length; i++) {
        const Move played_move = move_history->moves[i];
        if (played_move.start_square->y == starting_row) {
            if (played_move.start_square->x == king_x || played_move.start_square->x == rook_x) {
                return true;
            }
        }
    }
    return false;
}

static bool passes_through_check_when_castling(const Move* move, const Board* board) {
    Board board_copy = copy_board(board);
    const int row = move->start_square->y;
    const int start_x = move->start_square->x;
    const int end_x = move->end_square->x;
    const int direction = end_x - start_x > 0 ? 1 : -1;

    int x = start_x;
    Move submove;
    while (x != end_x) {
        submove.start_square = &board_copy.squares[row][x];
        submove.end_square = &board_copy.squares[row][x + 1];
        make_move(&submove, &board_copy);
        if (is_in_check(move->start_square->piece->color, &board_copy)) {
            deallocate_board(&board_copy);
            return true;
        }
        x += direction;
    }
    deallocate_board(&board_copy);
    return false;
}

static bool is_valid_castling_move(const Move* move, const MoveArray* move_history, Board* board) {
    const Color color = move->start_square->piece->color;
    const int starting_row = color == WHITE ? 7 : 0;
    const int king_x = 4;
    int rook_x;
    if (move->end_square->x == 6) {
        rook_x = 7;
    } else if (move->end_square->x == 2) {
        rook_x = 0;
    } else {
        return false;
    }

    if (has_castling_pieces_moved(move_history, starting_row, king_x, rook_x)) {
        return false;
    }
    if (!is_clear_line(&board->squares[starting_row][king_x], &board->squares[starting_row][rook_x], board)) {
        return false;
    }
    if (is_in_check(color, board)) {
        return false;
    }
    if (passes_through_check_when_castling(move, board)) {
        return false;
    }

    return true;
}

void make_castling_move(const Move* move, Board* board) {
    make_move(move, board);

    Move rook_move;
    const int row = move->start_square->y;
    const int move_direction = move->end_square->x - move->start_square->x; 
    if (move_direction > 0) {
        // kingside castling
        rook_move.start_square = &board->squares[row][7];
        rook_move.end_square = &board->squares[row][5];
    } else {
        // queenside castling
        rook_move.start_square = &board->squares[row][0];
        rook_move.end_square = &board->squares[row][3];
    }
    make_move(&rook_move, board);
}

bool is_legal_move(const Move* move, Board* board, const MoveArray* move_history) {
    if (is_castling_move(move)) {
        return is_valid_castling_move(move, move_history, board);
    }
    if (!validate_move_basic(move, board)) {
        return false;
    }
    if (leaves_king_in_check(move, board)) {
        return false;
    }

    return true;
}

char* move_to_uci_notation(const Move* move) {
    const char files[] = "abcdefgh";
    const char ranks[] = "87654321";

    char* uci_notation = malloc(5 * sizeof(char));
    sprintf(uci_notation, "%c%c%c%c",
            files[move->start_square->x],
            ranks[move->start_square->y],
            files[move->end_square->x],
            ranks[move->end_square->y]);
    return uci_notation;
}

Move uci_notation_to_move(const char* uci_notation, Board* board) {
    const int start_x = uci_notation[0] - 'a';
    const int start_y = 8 - (uci_notation[1] - '0');
    const int end_x = uci_notation[2] - 'a';
    const int end_y = 8 - (uci_notation[3] - '0');
    Move move;
    move.start_square = &board->squares[start_y][start_x];
    move.end_square = &board->squares[end_y][end_x];
    return move;
}
