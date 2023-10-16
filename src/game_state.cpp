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
            const std::shared_ptr<Piece>& piece = square.get_piece();
            if (piece && dynamic_cast<King*>(piece.get()) != nullptr && piece->get_color() == color) {
                return square;
            }
        }
    }
    return {};
}

static bool is_valid(const Move& move) {
    const bool end_square_has_same_color_piece = 
        move.get_end_square().get_piece() &&
        move.get_end_square().get_piece()->get_color() == move.get_start_square().get_piece()->get_color();

    if (end_square_has_same_color_piece) {
        return false;
    }
    return true;
}

static bool is_legal(const Move& psuedo_legal, const Board& board, const std::vector<Move>& move_history) {
    if (!is_valid(psuedo_legal)) {
        return false;
    }

    Move psuedo_legal_copy = psuedo_legal;
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;

    const Color player_to_move = board.get_player_to_move();
    psuedo_legal_copy.make_appropriate(board_copy, move_history_copy);
    if (is_in_check(player_to_move, board_copy, move_history_copy)) {
        return false;
    }
    return true;
}

static std::vector<Move> get_threatened_moves(const std::vector<std::reference_wrapper<const std::shared_ptr<Piece>>>& pieces, const Board& board, std::vector<Move> move_history) {
    std::vector<Move> legal_moves;
    for (const std::shared_ptr<Piece>& piece : pieces) {
        std::vector<Move> psuedo_legal_moves = piece->get_threatened_moves(board, move_history); 
        for (Move& move : psuedo_legal_moves) {
            if (is_valid(move)) {
                legal_moves.push_back(move);
            }
        }
    }
    return legal_moves;
}

std::vector<Move> get_all_threatened_moves(Color color, const Board& board, const std::vector<Move>& move_history) {
    std::vector<std::reference_wrapper<const std::shared_ptr<Piece>>> pieces;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            if (square.get_piece() && square.get_piece()->get_color() == color) {
                std::reference_wrapper<const std::shared_ptr<Piece>> piece = cref(square.get_piece());
                pieces.push_back(piece);
            } 
        }
    }
    return get_threatened_moves(pieces, board, move_history);
}

static std::vector<Move> get_legal_moves(const std::vector<std::reference_wrapper<const std::shared_ptr<Piece>>>& pieces, const Board& board, std::vector<Move> move_history) {
    std::vector<Move> legal_moves;
    for (const std::shared_ptr<Piece>& piece : pieces) {
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
    std::vector<std::reference_wrapper<const std::shared_ptr<Piece>>> pieces;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            if (square.get_piece() && square.get_piece()->get_color() == board.get_player_to_move()) {
                std::reference_wrapper<const std::shared_ptr<Piece>> piece = cref(square.get_piece());
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

bool is_in_check(Color color, const Board& board, const std::vector<Move>& move_history) {
    const std::optional<Square> king_square = get_king_square(color, board);
    if (!king_square.has_value()) {
        return false;  
    }

    const Color opponent = get_opposite_color(color);
    const std::vector<Move> psuedo_legal_moves = get_all_threatened_moves(opponent, board, move_history);
    for (const Move& move : psuedo_legal_moves) {
        const Square& threatened_square = move.get_end_square();
        if (threatened_square == king_square.value()) {
            return true;
        }
    }
    return false;
}

std::vector<std::reference_wrapper<const std::shared_ptr<Piece>>> get_all_pieces(Color color, const Board& board) {
    std::vector<std::reference_wrapper<const std::shared_ptr<Piece>>> pieces;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::shared_ptr<Piece>& piece = board.get_square(j, i).get_piece();
            if (piece && piece->get_color() == color) {
                pieces.push_back(piece);
            }
        }
    }
    return pieces;
}
