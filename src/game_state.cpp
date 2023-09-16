#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "board.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "square.h"

static std::optional<Square> get_king_square(Color color, const Board& board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            const std::optional<Piece>& piece = square.get_piece();
            if (piece.has_value() && piece.value().get_piece_type() == KING && piece.value().get_color() == color) {
                return square;
            }
        }
    }
    return {};
}

static std::vector<Square> get_threatened_squares(const Square& square, const Board& board) {
    std::vector<Square> threatened_squares;

    Move move;
    move.set_start_square(square);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.set_end_square(board.get_square(j, i));
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
            if (square.get_piece().has_value() && square.get_piece().value().get_color() == color) {
                const std::vector<Square> threatened_squares = get_threatened_squares(square, board);
                for (int i = 0; i < threatened_squares.size(); i++) {
                    all_threatened_squares.push_back(threatened_squares.at(i));
                }
            }
        }
    }
    return all_threatened_squares;
}

static std::vector<Move> get_legal_moves(const Square& square, const Board& board, const std::vector<Move>& move_history) {
    std::vector<Move> legal_moves;

    Move move;
    move.set_start_square(square);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            move.set_end_square(board.get_square(j, i));
            if (move.is_promotion_move(board)) {
                move.set_promotion_piece(QUEEN);
            } else {
                move.set_promotion_piece({});
            }
            if (move.is_legal(board, move_history)) {
                if (move.is_valid_promotion(board)) {
                    legal_moves.push_back(move);
                    const std::array<Piece_type, 3> other_promotion_pieces = {KNIGHT, BISHOP, ROOK};
                    for (int i = 0; i < other_promotion_pieces.size(); i++) {
                        move.set_promotion_piece(other_promotion_pieces.at(i));
                        legal_moves.push_back(move);
                    }
                } else {
                    legal_moves.push_back(move);
                }
            }
        }
    }
    return legal_moves;
}

std::vector<Move> get_all_legal_moves(const Board& board, const std::vector<Move>& move_history) {
    std::vector<Move> all_legal_moves;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Square& square = board.get_square(j, i);
            if (square.get_piece().has_value() && square.get_piece()->get_color() == board.get_player_to_move()) {
                const std::vector<Move> legal_moves = get_legal_moves(square, board, move_history);
                for (int i = 0; i < legal_moves.size(); i++) {
                    all_legal_moves.push_back(legal_moves.at(i));
                }
            }
        }
    }
    return all_legal_moves;
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

std::vector<Piece> get_all_pieces(Color color, const Board& board) {
    std::vector<Piece> pieces;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::optional<Piece>& piece = board.get_square(j, i).get_piece();
            if (piece.has_value() && piece.value().get_color() == color) {
                pieces.push_back(piece.value());
            }
        }
    }
    return pieces;
}
