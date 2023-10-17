#include "board_utils.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>


std::unique_ptr<Piece> King::clone() const {
    return std::make_unique<King>(*this);
}

char King::get_char_representation() const {
    return 'K';
}

std::vector<std::vector<int>> King::get_piece_square_table() const {
    return {
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40, 10,-30,-20},
        {-10,-20,-20,-20,-20, 10,-20,-10},
        { 20, 20,  0,  0,  0,  0,  5, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20},
    };
}

int King::get_value() const {
    return 200;
}

std::vector<Move> King::get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const {
    std::vector<Move> moves = this->get_threatened_moves(board);
    std::vector<Move> castling_moves = this->get_castling_moves(board, move_history);
    moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
    return moves;
}

std::vector<Move> King::get_threatened_moves(const Board& board) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());

    const std::array<std::pair<int, int>, 8> end_coordinates = {
        std::make_pair(start.get_x(), start.get_y() + 1),
        std::make_pair(start.get_x() + 1, start.get_y() + 1),
        std::make_pair(start.get_x() + 1, start.get_y()),
        std::make_pair(start.get_x() + 1, start.get_y() - 1),
        std::make_pair(start.get_x(), start.get_y() - 1),
        std::make_pair(start.get_x() - 1, start.get_y() - 1),
        std::make_pair(start.get_x() - 1, start.get_y()),
        std::make_pair(start.get_x() - 1, start.get_y() + 1)
    };
    for (std::pair<int, int> end_coordinate : end_coordinates) {
        try {
            const Square& end = board.get_square(
                    end_coordinate.first,
                    end_coordinate.second);
            moves.push_back(Move(start, end));
        } catch (const std::invalid_argument& e) {}
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

    if (!are_castling_pieces_on_initial_squares(move_history, starting_row, king_x, rook_x)) {
        return false;
    }
    
    const Square& king_square = board.get_square(king_x, starting_row);
    const Square& rook_square = board.get_square(rook_x, starting_row);
    if (!is_clear_line(king_square, rook_square, board)) {
        return false;
    }
    if (is_in_check(board.get_player_to_move(), board, move_history)) {
        return false;
    }
    if (passes_through_check_when_castling(move, board, move_history)) {
        return false;
    }

    return true;
}

std::vector<Move> King::get_castling_moves(const Board& board, const std::vector<Move>& move_history) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());

    const std::array<std::pair<int, int>, 2> castling_end_coordinates = {
        // kingside castling
        std::make_pair(start.get_x() + 2, start.get_y()),
        // queenside castling
        std::make_pair(start.get_x() - 2, start.get_y())
    };
    for (std::pair<int, int> end_coordinate : castling_end_coordinates) {
        try {
            const Square& end = board.get_square(
                    end_coordinate.first,
                    end_coordinate.second);
            Move castling_move = Move(start, end);
            if (is_valid_castling(castling_move, board, move_history)) {
                castling_move.set_castling_move(true);
                moves.push_back(castling_move);
            }
        } catch (const std::invalid_argument& e) {}
    }

    return moves;
}

bool King::are_castling_pieces_on_initial_squares(const std::vector<Move>& move_history, int starting_row, int king_x, int rook_x) const {
    for (int i = 0; i < move_history.size(); i++) {
        const Move& played_move = move_history.at(i);
        if (played_move.get_start_square().get_y() == starting_row) {
            if (played_move.get_start_square().get_x() == king_x || played_move.get_start_square().get_x() == rook_x) {
                return false;
            }
        }
        // check if rook has been captured while still on initial square
        if (played_move.get_end_square().get_x() == rook_x && played_move.get_end_square().get_y() == starting_row) {
            return false; 
        }
    }
    return true;
}

bool King::passes_through_check_when_castling(const Move& castling_move, const Board& board, const std::vector<Move>& move_history) const {
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;
    const int row = castling_move.get_start_square().get_y();
    const int start_x = castling_move.get_start_square().get_x();
    const int end_x = castling_move.get_end_square().get_x();
    const int direction = end_x - start_x > 0 ? 1 : -1;
    const Color player_to_move = board.get_player_to_move();

    int x = start_x;
    while (x != end_x) {
        const Square& start = board_copy.get_square(x, row); 
        const Square& end = board_copy.get_square(x + direction, row);
        Move submove = Move(start, end);
        submove.make_appropriate(board_copy, move_history_copy);
        if (is_in_check(player_to_move, board_copy, move_history_copy)) {
            return true;
        }
        x += direction;
    }
    return false;
}
