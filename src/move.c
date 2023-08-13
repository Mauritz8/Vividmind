#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "piece.h"
#include "game_state.h"

void move_array_push(MoveArray* move_array, const Move* move) {
    if (move_array->length == move_array->capacity) {
        move_array->capacity *= 2;
        move_array->moves = realloc(move_array->moves, move_array->capacity * sizeof(Move));
    }
    move_array->moves[move_array->length++] = *move;
}

MoveArray copy_move_array(const MoveArray* move_array) {
    MoveArray move_history_copy;
    move_history_copy.capacity = move_array->capacity;
    move_history_copy.moves = malloc(move_history_copy.capacity * sizeof(Move));
    move_history_copy.length = move_array->length;
    for (int i = 0; i < move_array->length; i++) {
        Move move;
        move.start_square = move_array->moves[i].start_square;
        move.end_square = move_array->moves[i].end_square;
        move.captured_piece = move_array->moves[i].captured_piece;
        move.is_castling_move = move_array->moves[i].is_castling_move;
        move.is_promotion = move_array->moves[i].is_promotion;
        move.promotion_piece = move_array->moves[i].promotion_piece;
        move.is_en_passant = move_array->moves[i].is_en_passant;
        move_history_copy.moves[i] = move;
    }
    return move_history_copy;
}

static void make_move(Move* move, Board* board) {
    Square* start_square = &board->squares[move->start_square->y][move->start_square->x]; 
    Square* end_square = &board->squares[move->end_square->y][move->end_square->x]; 
    if (end_square->piece) {
        move->captured_piece = end_square->piece;
    } else {
        move->captured_piece = NULL;
    }
    end_square->piece = start_square->piece;
    start_square->piece = NULL;
}

static void undo_move(Move* move, Board* board) {
    Square* start_square = &board->squares[move->start_square->y][move->start_square->x]; 
    Square* end_square = &board->squares[move->end_square->y][move->end_square->x]; 
    start_square->piece = end_square->piece;
    if (move->captured_piece) {
        end_square->piece = move->captured_piece;
    } else {
        end_square->piece = NULL;
    }
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

    const bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square_is_empty;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

static bool is_valid_pawn_move_threat(const Move* move, const Board* board) {
    const int direction = move->start_square->piece->color == BLACK ? 1 : -1;
    const int x_diff = move->end_square->x - move->start_square->x;
    const int y_diff = move->end_square->y - move->start_square->y;

    const bool is_threatening_diagonally = abs(x_diff) == 1 && y_diff == direction;

    if (is_threatening_diagonally) {
        return true;
    }
    return false;
}

static bool is_square_outside_board(const Square* square) {
    return square->x < 0 || square->x > 7 || square->y < 0 || square->y > 7;
}

static bool validate_move_basic(const Move* move, const Board* board) {
    if (move->start_square->piece->color != board->player_to_move) {
        return false;
    }

    if (is_square_outside_board(move->start_square) || is_square_outside_board(move->end_square)) {
        return false;
    }

    if (move->start_square->piece == NULL) {
        return false;
    }

    if (move->end_square->piece && move->start_square->piece->color == move->end_square->piece->color) {
        return false;
    }
    return true;
}

static bool is_valid_piece_movement(const Move* move, Board* board) {
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

bool validate_threatened_move(const Move* move, Board* board) {
    if (is_square_outside_board(move->start_square) || is_square_outside_board(move->end_square)) {
        return false;
    }

    if (move->start_square->piece == NULL) {
        return false;
    }

    switch (move->start_square->piece->piece_type) {
        case PAWN:
            return is_valid_pawn_move_threat(move, board);
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

bool leaves_king_in_check(const Move* move, const Board* board, const MoveArray* move_history) {
    Board board_copy = copy_board(board);
    MoveArray move_history_copy = copy_move_array(move_history);
    make_appropriate_move(move, &board_copy, &move_history_copy);
    if (is_check(&board_copy)) {
        deallocate_board(&board_copy);
        return true;
    }
    deallocate_board(&board_copy);
    return false;
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

static bool passes_through_check_when_castling(const Move* move, const Board* board, const MoveArray* move_history) {
    Board board_copy = copy_board(board);
    MoveArray move_history_copy = copy_move_array(move_history);
    const int row = move->start_square->y;
    const int start_x = move->start_square->x;
    const int end_x = move->end_square->x;
    const int direction = end_x - start_x > 0 ? 1 : -1;

    int x = start_x;
    Move submove;
    while (x != end_x) {
        submove.start_square = &board_copy.squares[row][x];
        submove.end_square = &board_copy.squares[row][x + direction];
        make_appropriate_move(&submove, &board_copy, &move_history_copy);
        if (is_check(&board_copy)) {
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

    if (move->end_square->y != starting_row) {
        return false;
    }
    const int king_x = 4;
    if (move->start_square->x != king_x || move->start_square->y != starting_row) {
        return false;
    }
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
    if (is_check(board)) {
        return false;
    }
    if (passes_through_check_when_castling(move, board, move_history)) {
        return false;
    }

    return true;
}

static Move get_castling_rook_move(const Move* castling_king_move, Board* board) {
    Move rook_move;
    const int row = castling_king_move->start_square->y;
    const int move_direction = castling_king_move->end_square->x - castling_king_move->start_square->x; 
    if (move_direction > 0) {
        // kingside castling
        rook_move.start_square = &board->squares[row][7];
        rook_move.end_square = &board->squares[row][5];
    } else {
        // queenside castling
        rook_move.start_square = &board->squares[row][0];
        rook_move.end_square = &board->squares[row][3];
    }
    return rook_move;
}

static void make_castling_move(const Move* move, Board* board) {
    make_move(move, board);
    Move rook_move = get_castling_rook_move(move, board);
    make_move(&rook_move, board);
}

static void undo_castling_move(const Move* move, Board* board) {
    undo_move(move, board);
    Move rook_move = get_castling_rook_move(move, board);
    undo_move(&rook_move, board);
}

static bool is_valid_en_passant_move(const Move* move, Board* board, const MoveArray* move_history) {
    if (move->start_square->piece->piece_type != PAWN) {
        return false;
    }
    const int direction = move->start_square->piece->color == BLACK ? 1 : -1;
    const int x_diff = move->end_square->x - move->start_square->x;
    const int y_diff = move->end_square->y - move->start_square->y;

    const bool is_diagonal_pawn_move = abs(x_diff) == 1 && y_diff == direction;
    const Piece* adjacent_piece = board->squares[move->start_square->y][move->start_square->x + x_diff].piece;
    const bool has_pawn_adjacent = adjacent_piece && adjacent_piece->piece_type == PAWN;
    const bool is_adjacent_pawn_opponents_piece = adjacent_piece && adjacent_piece->color != move->start_square->piece->color;

    if (move_history->length == 0) {
        return false;
    }
    const Move previous_move = move_history->moves[move_history->length - 1];
    const int y_diff_previous_move = previous_move.end_square->y - previous_move.start_square->y;

    return is_diagonal_pawn_move &&
           has_pawn_adjacent &&
           is_adjacent_pawn_opponents_piece &&
           abs(y_diff_previous_move) == 2;
}

static void make_en_passant_move(Move* move, Board* board) {
    Square* start_square = &board->squares[move->start_square->y][move->start_square->x];
    Square* end_square = &board->squares[move->end_square->y][move->end_square->x];
    end_square->piece = start_square->piece;
    start_square->piece = NULL;

    const int x_diff = move->end_square->x - move->start_square->x;
    Square* captured_square = &board->squares[move->start_square->y][move->start_square->x + x_diff];
    move->captured_piece = captured_square->piece;
    captured_square->piece = NULL;
}

static void undo_en_passant_move(Move* move, Board* board) {
    Square* start_square = &board->squares[move->start_square->y][move->start_square->x];
    Square* end_square = &board->squares[move->end_square->y][move->end_square->x];
    start_square->piece = end_square->piece;
    end_square->piece = NULL;

    const int x_diff = move->end_square->x - move->start_square->x;
    Square* captured_square = &board->squares[move->start_square->y][move->start_square->x + x_diff];
    captured_square->piece = move->captured_piece;
}

bool is_promotion_move(const Move* move, Board* board) {
    const Piece* piece = move->start_square->piece;
    const Piece_type piece_type = piece->piece_type;
    const int y_end = move->end_square->y;
    const int promotion_row = piece->color == WHITE ? 0 : 7;

    if (piece_type == PAWN && y_end == promotion_row) {
        return true;
    }
    return false;
}

bool is_valid_promotion_move(const Move* move, Board* board) {
    const bool is_correct_promotion_piece = 
        move->promotion_piece == KNIGHT ||
        move->promotion_piece == BISHOP ||
        move->promotion_piece == ROOK ||
        move->promotion_piece == QUEEN;


    if (is_promotion_move(move, board) && is_valid_pawn_move(move, board) && is_correct_promotion_piece) {
        return true;
    }
    return false;
}

static void make_promotion_move(const Move* move, Board* board) {
    make_move(move, board);
    move->end_square->piece->piece_type = move->promotion_piece;
}

static void undo_promotion_move(const Move* move, Board* board) {
    undo_move(move, board);
    move->start_square->piece->piece_type = PAWN;
}

bool is_legal_move(const Move* move, Board* board, const MoveArray* move_history) {
    if (!validate_move_basic(move, board)) {
        return false;
    }
    if (leaves_king_in_check(move, board, move_history)) {
        return false;
    }

    if (is_valid_promotion_move(move, board)) {
        return true;
    }
    if (is_valid_castling_move(move, move_history, board)) {
        return true;
    }
    if (is_valid_en_passant_move(move, board, move_history)) {
        return true;
    }

    if (!is_valid_piece_movement(move, board)) {
        return false;
    }

    return true;
}

void make_appropriate_move(Move* move, Board* board, MoveArray* move_history) {
    if (is_valid_castling_move(move, move_history, board)) {
        move->is_castling_move = true;
        move->is_promotion = false;
        move->is_en_passant = false;
        make_castling_move(move, board);
    } else if (is_valid_en_passant_move(move, board, move_history)) {
        move->is_en_passant = true;
        move->is_castling_move = false;
        move->is_promotion = false;
        make_en_passant_move(move, board);
    } else if (is_valid_promotion_move(move, board)) {
        move->is_promotion = true;
        move->is_castling_move = false;
        move->is_en_passant = false;
        make_promotion_move(move, board);
    } else {
        move->is_castling_move = false;
        move->is_promotion = false;
        move->is_en_passant = false;
        make_move(move, board);
    }

    move_array_push(move_history, move);
    switch_player_to_move(board);
}

void undo_appropriate_move(Move* move, Board* board, MoveArray* move_history) {
    if (move->is_castling_move) {
        undo_castling_move(move, board);
    } else if (move->is_en_passant) {
        undo_en_passant_move(move, board);
    } else if (move->is_promotion) {
        undo_promotion_move(move, board);
    } else {
        undo_move(move, board);
    }

    move_history->length--;
    switch_player_to_move(board);
}

char* move_to_uci_notation(const Move* move) {
    const char files[] = "abcdefgh";
    const char ranks[] = "87654321";

    char* uci_notation = malloc(6 * sizeof(char));
    sprintf(uci_notation, "%c%c%c%c",
            files[move->start_square->x],
            ranks[move->start_square->y],
            files[move->end_square->x],
            ranks[move->end_square->y]);
    if (move->promotion_piece) {
        uci_notation[4] = tolower(get_char_representation(move->promotion_piece));
        uci_notation[5] = '\0';
    }
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

    const char promotion_piece = uci_notation[4];
    if (promotion_piece) {
        move.promotion_piece = get_promotion_piece_type(promotion_piece);
    }
    return move;
}
