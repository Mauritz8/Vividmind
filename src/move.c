#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "piece.h"

void make_move(Move* move, Board* board) {
    Square* start_square = &board->squares[move->start_square->y][move->start_square->x]; 
    Square* end_square = &board->squares[move->end_square->y][move->end_square->x]; 
    if (end_square->piece) {
        free(end_square->piece);
    }
    end_square->piece = start_square->piece;
    start_square->piece = NULL;
}

static bool is_same_line(Square* square1, Square* square2) {
    return square1->x == square2->x || square1->y == square2->y;
}

static bool is_same_diagonal(Square* square1, Square* square2) {
    return abs(square1->x - square2->x) == abs(square1->y - square2->y);
}

static bool is_clear_line(Square* square1, Square* square2, Board* board) {
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

static bool is_clear_diagonal(Square* square1, Square* square2, Board *board) {
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

static bool is_valid_rook_move(Move* move, Board* board) {
    if (!is_same_line(move->start_square, move->end_square)) {
        return false;
    }
    if (!is_clear_line(move->start_square, move->end_square, board)) {
        return false;
    }
    return true;
}

static bool is_valid_bishop_move(Move* move, Board* board) {
    if (!is_same_diagonal(move->start_square, move->end_square)) {
        return false;
    }
    if (!is_clear_diagonal(move->start_square, move->end_square, board)) {
        return false;
    }
    return true;
}

static bool is_valid_queen_move(Move* move, Board* board) {
    if (is_valid_rook_move(move, board)) {
        return true;
    }
    if (is_valid_bishop_move(move, board)) {
        return true;
    }
    return false;
}

static bool is_valid_knight_move(Move* move) {
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

static bool is_valid_king_move(Move* move) {
    int x_diff = abs(move->start_square->x - move->end_square->x);
    int y_diff = abs(move->start_square->y - move->end_square->y);
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

static bool is_valid_pawn_move(Move* move, Board* board) {
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

static bool is_square_outside_board(Square* square) {
    return square->x < 0 || square->x > 7 || square->y < 0 || square->y > 7;
}

static bool validate_move_basic(Move* move, Board* board) {
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

static Square* get_king_square(Color color, Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->squares[i][j].piece;
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

static MoveArray get_all_legal_moves(Color color, Board* board) {
    MoveArray all_legal_moves;
    int capacity = 16;
    all_legal_moves.moves = malloc(capacity * sizeof(Move));
    all_legal_moves.length = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square* square = &board->squares[i][j];
            if (square->piece && square->piece->color == color) {
                MoveArray legal_moves = get_legal_moves(square, board);
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

static bool is_in_check(Color color, Board* board) {
    Square* king_square = get_king_square(color, board);

    Color opponent_color = color == WHITE ? BLACK : WHITE;
    MoveArray opponent_moves = get_all_legal_moves(opponent_color, board);
    for (int i = 0; i < opponent_moves.length; i++) {
        if (opponent_moves.moves[i].end_square == king_square) {
            free(opponent_moves.moves);
            return true;
        }
    }
    free(opponent_moves.moves);
    return false;
}

static bool leaves_king_in_check(Move* move, Board* board) {
    Board board_copy = copy_board(board);
    Color color_to_move = move->start_square->piece->color;
    make_move(move, &board_copy);
    if (is_in_check(color_to_move, &board_copy)) {
        deallocate_board(&board_copy);
        return true;
    }
    deallocate_board(&board_copy);
    return false;
}

bool is_checkmated(Color color, Board* board) {
   if (!is_in_check(color, board)) {
       return false;
   }

   MoveArray moves = get_all_legal_moves(color, board);
   for (int i = 0; i < moves.length; i++) {
       if (!leaves_king_in_check(&moves.moves[i], board)) {
           free(moves.moves);
           return false;
       }
   }
   free(moves.moves);
   return true;
}

bool is_castling_move(Move* move) {
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

static bool has_castling_pieces_moved(MoveArray* move_history, int starting_row, int king_x, int rook_x) {
    for (int i = 0; i < move_history->length; i++) {
        Move played_move = move_history->moves[i];
        if (played_move.start_square->y == starting_row) {
            if (played_move.start_square->x == king_x || played_move.start_square->x == rook_x) {
                return true;
            }
        }
    }
    return false;
}

static bool passes_through_check_when_castling(Move* move, Board* board) {
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

static bool is_valid_castling_move(Move* move, MoveArray* move_history, Board* board) {
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

void make_castling_move(Move* move, Board* board) {
    make_move(move, board);

    Move rook_move;
    int row = move->start_square->y;
    if (move->end_square->x > move->start_square->x) {
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

bool is_legal_move(Move* move, Board* board, MoveArray* move_history) {
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

char* move_to_uci_notation(Move* move) {
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

Move uci_notation_to_move(char* uci_notation, Board* board) {
    int start_x = uci_notation[0] - 'a';
    int start_y = 8 - (uci_notation[1] - '0');
    int end_x = uci_notation[2] - 'a';
    int end_y = 8 - (uci_notation[3] - '0');
    Move move;
    move.start_square = &board->squares[start_y][start_x];
    move.end_square = &board->squares[end_y][end_x];
    return move;
}
