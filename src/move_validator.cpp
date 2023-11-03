#include "move_validator.hpp"


MoveValidator::MoveValidator(const Board& board) 
    : board(board)
{}

bool MoveValidator::is_valid_rook_move(const Move& move) const {
    if (!is_same_line(move.start, move.end)) {
        return false;
    }
    if (!is_clear_line(move.start, move.end)) {
        return false;
    }
    return true;
}

bool MoveValidator::is_valid_bishop_move(const Move& move) const {
    if (!is_same_diagonal(move.start, move.end)) {
        return false;
    }
    if (!is_clear_diagonal(move.start, move.end)) {
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
    if (abs(move.end % 8 - move.start % 8) == 1 && abs(move.end - move.start) / 8 == 2) {
        return true;
    }
    if (abs(move.end % 8 - move.start % 8) == 2 && abs(move.end - move.start) / 8 == 1) {
        return true;
    }
    return false;
}

bool MoveValidator::is_valid_king_move(const Move& move) const {
    const int x_diff = abs(move.start % 8 - move.end % 8);
    const int y_diff = abs(move.start - move.end) / 8;
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

bool MoveValidator::is_valid_castling_move(const Move& move) const {
    const int king_x = 4;
    if (move.start % 8 != king_x) {
        return false;
    }
    int end_x  = move.end % 8;
    if (end_x != 2 && end_x != 6) {
        return false;
    }

    return true;
}

bool MoveValidator::is_valid_pawn_move(const Move& move) const {
    const Square& start_square = board.squares[move.start];
    const Square& end_square = board.squares[move.end];
    
    const int direction = start_square.piece->color == BLACK ? 1 : -1;

    const int x_diff = move.end % 8 - move.start % 8;
    const int y_diff = (move.end - move.start) / 8;

    const bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && !end_square.piece;

    const int starting_row = start_square.piece->color == BLACK ? 1 : 6;
    const bool is_on_starting_row = move.start / 8 == starting_row;
    const bool one_square_forward_is_empty = !board.squares[move.start + 8 * direction].piece;
    const bool is_valid_move_two_squares_forward = x_diff == 0 && y_diff == 2 * direction && is_on_starting_row && one_square_forward_is_empty && !end_square.piece;

    const bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square.piece;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

bool MoveValidator::is_valid_pawn_move_threat(const Move& move) const {
    const int direction = board.squares[move.start].piece->color == BLACK ? 1 : -1;
    const int x_diff = move.end % 8 - move.start % 8;
    const int y_diff = (move.end - move.start) / 8;

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
    int end_y = pawn_move.end / 8;
    if (end_y == 0 || end_y == 7) {
        return true;
    }
    return false;
}

bool MoveValidator::is_clear_line(int pos1, int pos2) const {
    int x_diff = pos2 % 8 - pos1 % 8;
    int y_diff = pos2 / 8 - pos1 / 8;
    int x_direction = x_diff > 0 ? 1 : -1;
    int y_direction = y_diff > 0 ? 1 : -1;

    int step = x_direction + y_diff * 8;
    int pos = pos1 + step;
    while (pos != pos2) {
        if (board.squares[pos].piece) {
            return false;
        }
        pos += step;
    }
    return true;
}

bool MoveValidator::is_clear_diagonal(int pos1, int pos2) const {
    if (pos1 == pos2) {
        return true;
    }

    int x_diff = pos2 % 8 - pos1 % 8;
    int y_diff = pos2 / 8 - pos1 / 8;
    const int x_direction = x_diff > 0 ? 1 : -1;
    const int y_direction = y_diff > 0 ? 1 : -1;

    int step = x_direction + y_diff * 8;
    int pos = pos1 + step;
    while (pos != pos2) {
        if (board.squares[pos].piece) {
            return false;
        }
        pos += step;
    }
    return true;
}

bool MoveValidator::is_same_line(int pos1, int pos2) const {
    return abs(pos1 % 8 - pos2 % 8) == 0 || pos1 / 8 == pos2 / 8;
}

bool MoveValidator::is_same_diagonal(int pos1, int pos2) const {
    return abs(pos1 % 8 - pos2 % 8) == (pos1 - pos2) / 8;
}
