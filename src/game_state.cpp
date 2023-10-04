#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "board.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include "square.h"

static std::optional<Square> get_king_square(Color color, const Board& board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            const std::unique_ptr<Piece>& piece = square.get_piece();
            if (piece && dynamic_cast<King*>(piece.get()) != nullptr && piece->get_color() == color) {
                return square;
            }
        }
    }
    return {};
}

static std::vector<Square> get_threatened_squares(const Square& square, const Board& board) {
    std::vector<Square> threatened_squares;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move = Move(square, board.get_square(j, i));
            if (move.is_threatened_move(board)) {
                threatened_squares.push_back(move.get_end_square());
            }
        }
    }
    return threatened_squares;
}

std::vector<Square> get_all_threatened_squares(Color color, const Board& board) {
    std::vector<Square> all_threatened_squares;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            if (square.get_piece() && square.get_piece()->get_color() == color) {
                const std::vector<Square> threatened_squares = get_threatened_squares(square, board);
                for (int i = 0; i < threatened_squares.size(); i++) {
                    all_threatened_squares.push_back(threatened_squares.at(i));
                }
            }
        }
    }
    return all_threatened_squares;
}

static std::vector<Move> get_legal_moves(const std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>>& pieces, const Board& board, std::vector<Move> move_history) {
    std::vector<Move> legal_moves;
    for (const std::unique_ptr<Piece>& piece : pieces) {
        std::vector<Move> piece_legal_moves = piece->get_legal_moves(board, move_history); 
        for (Move& move : piece_legal_moves) {
            legal_moves.push_back(move);
        }
    }
    return legal_moves;
}

std::vector<Move> get_all_legal_moves(const Board& board, const std::vector<Move>& move_history) {
    std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>> pieces;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            if (square.get_piece() && square.get_piece()->get_color() == board.get_player_to_move()) {
                std::reference_wrapper<const std::unique_ptr<Piece>> piece = cref(square.get_piece());
                pieces.push_back(piece);
            } 
        }
    }
    std::vector<Move> legal_moves = get_legal_moves(pieces, board, move_history);
    return legal_moves;
}

Color get_opposite_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}

bool is_check(const Board& board) {
    const Color opponent_color = get_opposite_color(board.get_player_to_move());
    const std::optional<Square> opponent_king_square = get_king_square(opponent_color, board);
    if (!opponent_king_square.has_value()) {
        return false;  
    }

    const std::vector<Square> threatened_squares = get_all_threatened_squares(board.get_player_to_move(), board);
    for (int i = 0; i < threatened_squares.size(); i++) {
        const Square& threatened_square = threatened_squares.at(i);
        if (threatened_square == opponent_king_square.value()) {
            return true;
        }
    }
    return false;
}

std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>> get_all_pieces(Color color, const Board& board) {
    std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>> pieces;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::unique_ptr<Piece>& piece = board.get_square(j, i).get_piece();
            if (piece && piece->get_color() == color) {
                pieces.push_back(piece);
            }
        }
    }
    return pieces;
}
