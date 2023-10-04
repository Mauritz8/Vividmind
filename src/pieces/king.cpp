#include "board_utils.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include <algorithm>
#include <array>
#include <vector>


std::unique_ptr<Piece> King::clone() const {
    return std::make_unique<King>(*this);
}

char King::get_char_representation() const {
    return 'K';
}

std::vector<Move> King::get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());
    const std::array<Square, 8>& end_squares = {
        board.get_square(start.get_x(), start.get_y() + 1),
        board.get_square(start.get_x() + 1, start.get_y() + 1),
        board.get_square(start.get_x() + 1, start.get_y()),
        board.get_square(start.get_x() + 1, start.get_y() - 1),
        board.get_square(start.get_x(), start.get_y() - 1),
        board.get_square(start.get_x() - 1, start.get_y() - 1),
        board.get_square(start.get_x() - 1, start.get_y()),
        board.get_square(start.get_x() - 1, start.get_y() + 1)
    };
    for (const Square& end : end_squares) {
        moves.push_back(Move(start, end));
    }

    Move kingside_castling = Move(start, board.get_square(start.get_x() + 2, start.get_y()));
    Move queenside_castling = Move(start, board.get_square(start.get_x() - 2, start.get_y()));
    std::array<Move, 2> castling_moves = {std::move(kingside_castling), std::move(queenside_castling)};
    for (Move& castling_move : castling_moves) {
        if (is_valid_castling(castling_move, board, move_history)) {
            castling_move.set_castling_move(true);
            moves.push_back(castling_move);
        }
    }

    return moves;
}

bool King::is_valid_castling(const Move& move, const Board& board, const std::vector<Move>& move_history) const {
    const int starting_row = this->get_color() == WHITE ? 7 : 0;
    const int king_x = 4;
    if (this->get_x() != king_x || this->get_y() != starting_row) {
        return false;
    }
    int rook_x;
    if (move.get_end_square().get_x() == 6) {
        rook_x = 7;
    } else if (move.get_end_square().get_x() == 2) {
        rook_x = 0;
    } else {
        return false;
    }

    if (has_castling_pieces_moved(move_history, starting_row, king_x, rook_x)) {
        return false;
    }
    
    const Square& king_square = board.get_square(king_x, starting_row);
    const Square& rook_square = board.get_square(rook_x, starting_row);
    if (!is_clear_line(king_square, rook_square, board)) {
        return false;
    }
    if (is_check(board, move_history)) {
        return false;
    }
    if (passes_through_check_when_castling(move, board, move_history)) {
        return false;
    }

    return true;
}

bool King::has_castling_pieces_moved(const std::vector<Move>& move_history, int starting_row, int king_x, int rook_x) const {
    for (int i = 0; i < move_history.size(); i++) {
        const Move& played_move = move_history.at(i);
        if (played_move.get_start_square().get_y() == starting_row) {
            if (played_move.get_start_square().get_x() == king_x || played_move.get_start_square().get_x() == rook_x) {
                return true;
            }
        }
    }
    return false;
}

bool King::passes_through_check_when_castling(const Move& castling_move, const Board& board, const std::vector<Move>& move_history) const {
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;
    const int row = castling_move.get_start_square().get_y();
    const int start_x = castling_move.get_start_square().get_x();
    const int end_x = castling_move.get_end_square().get_x();
    const int direction = end_x - start_x > 0 ? 1 : -1;

    int x = start_x;
    while (x != end_x) {
        const Square& start = board_copy.get_square(x, row); 
        const Square& end = board_copy.get_square(x + direction, row);
        Move submove = Move(start, end);
        submove.make_appropriate(board_copy, move_history_copy);
        if (is_check(board_copy, move_history_copy)) {
            return true;
        }
        x += direction;
    }
    return false;
}
