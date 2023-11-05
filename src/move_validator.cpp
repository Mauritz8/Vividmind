#include "move_validator.hpp"


MoveValidator::MoveValidator(const Board& board) 
    : board(board)
{}

bool MoveValidator::is_valid_en_passant(const Move& pawn_capture) const {
    if (!board.game_state.en_passant_square.has_value()) {
        return false;
    }
    return pawn_capture.end == board.game_state.en_passant_square.value();
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
