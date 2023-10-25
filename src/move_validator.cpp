#include "move_validator.h"

#include "board_utils.h"


MoveValidator::MoveValidator(const Board& board, BoardHelper& board_helper) 
    : board(board)
    , board_helper(board_helper)
{}

bool MoveValidator::is_valid_rook_move(const Move& move) const {
    if (!is_same_line(move.start, move.end)) {
        return false;
    }
    if (!board_helper.is_clear_line(move.start, move.end)) {
        return false;
    }
    return true;
}

bool MoveValidator::is_valid_bishop_move(const Move& move) const {
    if (!is_same_diagonal(move.start, move.end)) {
        return false;
    }
    if (!board_helper.is_clear_diagonal(move.start, move.end)) {
        return false;
    }
    return true;
}

bool MoveValidator::is_valid_queen_move(const Move& move) const {
    if (is_valid_rook_move(move)) {
        return true;
    }
    if (is_valid_bishop_move(move)) {
        return true;
    }
    return false;
}

bool MoveValidator::is_valid_knight_move(const Move& move) const {
    if (abs(move.end.x - move.start.x) == 1 && abs(move.end.y - move.start.y) == 2) {
        return true;
    }
    if (abs(move.end.x - move.start.x) == 2 && abs(move.end.y - move.start.y) == 1) {
        return true;
    }
    return false;
}

bool MoveValidator::is_valid_king_move(const Move& move) const {
    const int x_diff = abs(move.start.x - move.end.x);
    const int y_diff = abs(move.start.y - move.end.y);
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

bool MoveValidator::is_valid_castling_move(const Move& move) const {
    const int king_x = 4;
    if (move.start.x != king_x) {
        return false;
    }
    if (move.end.x != 2 && move.end.x != 6) {
        return false;
    }

    return true;
}

bool MoveValidator::is_valid_pawn_move(const Move& move) const {
    const Square& start_square = board.get_square(move.start);
    const Square& end_square = board.get_square(move.end);
    
    const int direction = start_square.piece->color == BLACK ? 1 : -1;

    const int x_diff = move.end.x - move.start.x;
    const int y_diff = move.end.y - move.start.y;

    const bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && !end_square.piece;

    const int starting_row = start_square.piece->color == BLACK ? 1 : 6;
    const bool is_on_starting_row = start_square.y == starting_row;
    const bool one_square_forward_is_empty = !board.get_square(move.start.x, move.start.y + direction).piece;
    const bool is_valid_move_two_squares_forward = x_diff == 0 && y_diff == 2 * direction && is_on_starting_row && one_square_forward_is_empty && !end_square.piece;

    const bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square.piece;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

bool MoveValidator::is_valid_pawn_move_threat(const Move& move) const {
    const int direction = board.get_square(move.start).piece->color == BLACK ? 1 : -1;
    const int x_diff = move.end.x - move.start.x;
    const int y_diff = move.end.y - move.start.y;

    const bool is_threatening_diagonally = abs(x_diff) == 1 && y_diff == direction;

    if (is_threatening_diagonally) {
        return true;
    }
    return false;
}

bool MoveValidator::is_valid_en_passant(const Move& pawn_capture) const {
    if (!board.game_state.en_passant_square.has_value()) {
        return false;
    }
    return pawn_capture.end == board.game_state.en_passant_square.value();
}

bool MoveValidator::is_promotion_move(const Move& pawn_move) const {
    if (pawn_move.end.y == 0 || pawn_move.end.y == 7) {
        return true;
    }
    return false;
}
