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

static Pos get_king_square(Color color, const Board& board) {
    for (auto& piece : board.game_state.pieces[color]) {
        if (piece->piece_type == KING) {
            return Pos{piece->x, piece->y};
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}

static bool is_valid(const Move& move, const Board& board) {
    const Square& start = board.get_square(move.start.x, move.start.y);
    const Square& end = board.get_square(move.end.x, move.end.y);
    const bool end_square_has_same_color_piece = 
        end.piece &&
        end.piece->color == start.piece->color;

    if (end_square_has_same_color_piece) {
        return false;
    }
    return true;
}

static bool is_legal(Move& psuedo_legal, Board& board) {
    if (!is_valid(psuedo_legal, board)) {
        return false;
    }

    const Color player_to_move = board.game_state.player_to_move;
    psuedo_legal.make_appropriate(board);
    if (is_in_check(player_to_move, board)) {
        psuedo_legal.undo_appropriate(board);
        return false;
    }
    psuedo_legal.undo_appropriate(board);
    return true;
}

std::vector<Move> get_threatened_moves(Color color, Board& board) {
    std::vector<Move> legal_moves = board.get_threatened_moves(color);
    for (auto it = legal_moves.begin(); it != legal_moves.end();) {
        if (!is_valid(*it, board)) {
            it = legal_moves.erase(it);
        } else {
            ++it;
        }
    }
    return legal_moves;
}

std::vector<Move> get_legal_moves(Board& board) {
    std::vector<Move> legal_moves = board.get_psuedo_legal_moves();
    for (auto it = legal_moves.begin(); it != legal_moves.end();) {
        if (!is_legal(*it, board)) {
            it = legal_moves.erase(it);
        } else {
            ++it;
        }
    }
    return legal_moves;
}

Color get_opposite_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}

bool is_in_check(Color color, Board& board) {
    const Pos king_square = get_king_square(color, board);
    const Color opponent = get_opposite_color(color);
    const std::vector<Move> psuedo_legal_moves = get_threatened_moves(opponent, board);
    for (const Move& move : psuedo_legal_moves) {
        if (move.end == king_square) {
            return true;
        }
    }
    return false;
}
