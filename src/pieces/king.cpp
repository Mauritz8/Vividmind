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

std::vector<Move> King::get_psuedo_legal_moves(const Board& board) const {
    std::vector<Move> moves = this->get_threatened_moves(board);
    std::vector<Move> castling_moves = this->get_castling_moves(board);
    moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
    return moves;
}

std::vector<Move> King::get_threatened_moves(const Board& board) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());

    const std::array<Pos, 8> end_squares = {
        Pos{start.get_x(), start.get_y() + 1},
        Pos{start.get_x() + 1, start.get_y() + 1},
        Pos{start.get_x() + 1, start.get_y()},
        Pos{start.get_x() + 1, start.get_y() - 1},
        Pos{start.get_x(), start.get_y() - 1},
        Pos{start.get_x() - 1, start.get_y() - 1},
        Pos{start.get_x() - 1, start.get_y()},
        Pos{start.get_x() - 1, start.get_y() + 1}
    };
    for (Pos end_pos : end_squares) {
        try {
            const Square& end = board.get_square(
                    end_pos.x,
                    end_pos.y);
            moves.push_back(Move(start, end));
        } catch (const std::invalid_argument& e) {}
    }

    return moves;
}

bool King::is_valid_castling(const Move& move, const Board& board) const {
    int rook_x;
    if (move.end.x == 6) {
        rook_x = 7;
    } else if (move.end.x == 2) {
        rook_x = 0;
    } 
    
    const Pos king_pos = Pos{move.start.x, move.start.y};
    const Pos rook_pos = Pos{rook_x, move.start.y};
    if (!is_clear_line(king_pos, rook_pos, board)) {
        return false;
    }
    if (is_in_check(board.player_to_move, board)) {
        return false;
    }
    if (passes_through_check_when_castling(move, board)) {
        return false;
    }

    return true;
}

std::vector<Move> King::get_castling_moves(const Board& board) const {
    std::vector<Move> castling_moves = get_potential_castling_moves(board);
    for (auto it = castling_moves.begin(); it != castling_moves.end();) {
        if (!is_valid_castling(*it, board)) {
            it = castling_moves.erase(it);
        } else {
            it->is_castling_move = true;
            ++it;
        }
    }
    return castling_moves;
}

std::vector<Move> King::get_potential_castling_moves(const Board& board) const {
    const Color color = this->get_color();
    const int king_x = 4;
    const int row = this->get_color() == WHITE ? 7 : 0;
    const int kingside_end_x = 6;
    const int queenside_end_x = 2;

    std::vector<Move> potential_castling_moves;
    if (color == WHITE && board.castling_rights.white_kingside ||
        color == BLACK && board.castling_rights.black_kingside) {
        potential_castling_moves.push_back(Move(king_x, row, kingside_end_x, row));
    } 

    if (color == WHITE && board.castling_rights.white_queenside ||
        color == BLACK && board.castling_rights.black_queenside) {
        potential_castling_moves.push_back(Move(king_x, row, queenside_end_x, row));
    }

    return potential_castling_moves;
}

bool King::passes_through_check_when_castling(const Move& castling_move, const Board& board) const {
    Board board_copy = board;
    const int row = castling_move.start.y;
    const int start_x = castling_move.start.x;
    const int end_x = castling_move.end.x;
    const int direction = end_x - start_x > 0 ? 1 : -1;
    const Color player_to_move = board.player_to_move;

    int x = start_x;
    while (x != end_x) {
        Move submove = Move(x, row, x + direction, row);
        submove.make_appropriate(board_copy);
        if (is_in_check(player_to_move, board_copy)) {
            return true;
        }
        x += direction;
    }
    return false;
}
