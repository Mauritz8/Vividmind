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

static bool is_psuedo_legal(const Move& psuedo_legal) {
    const bool end_square_has_same_color_piece = 
        psuedo_legal.get_end_square().get_piece() &&
        psuedo_legal.get_end_square().get_piece()->get_color() == psuedo_legal.get_start_square().get_piece()->get_color();

    if (psuedo_legal.get_end_square().is_outside_board() || end_square_has_same_color_piece) {
        return false;
    }
    return true;
}

static bool is_legal(const Move& psuedo_legal, const Board& board, const std::vector<Move>& move_history) {
    Move psuedo_legal_copy = psuedo_legal;
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;

    psuedo_legal_copy.make_appropriate(board_copy, move_history_copy);
    if (is_psuedo_legal(psuedo_legal) || is_check(board_copy)) {
        return false;
    }
    return true;
}

static std::vector<Move> get_psuedo_legal_moves(const std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>>& pieces, const Board& board, std::vector<Move> move_history) {
    std::vector<Move> legal_moves;
    for (const std::unique_ptr<Piece>& piece : pieces) {
        std::vector<Move> psuedo_legal_moves = piece->get_psuedo_legal_moves(board, move_history); 
        for (Move& move : psuedo_legal_moves) {
            if (is_psuedo_legal(move)) {
                legal_moves.push_back(move);
            }
        }
    }
    return legal_moves;
}

std::vector<Move> get_all_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) {
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
    std::vector<Move> psuedo_legal_moves = get_psuedo_legal_moves(pieces, board, move_history);
    return psuedo_legal_moves;
}

static std::vector<Move> get_legal_moves(const std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>>& pieces, const Board& board, std::vector<Move> move_history) {
    std::vector<Move> legal_moves;
    for (const std::unique_ptr<Piece>& piece : pieces) {
        std::vector<Move> psuedo_legal_moves = piece->get_psuedo_legal_moves(board, move_history); 
        for (Move& move : psuedo_legal_moves) {
            if (is_legal(move, board, move_history)) {
                legal_moves.push_back(move);
            }
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

bool is_check(const Board& board, const std::vector<Move>& move_history) {
    const Color opponent_color = get_opposite_color(board.get_player_to_move());
    const std::optional<Square> opponent_king_square = get_king_square(opponent_color, board);
    if (!opponent_king_square.has_value()) {
        return false;  
    }

    const std::vector<Move> psuedo_legal_moves = get_all_psuedo_legal_moves(board, move_history);
    for (const Move& move : psuedo_legal_moves) {
        const Square& threatened_square = move.get_end_square();
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
