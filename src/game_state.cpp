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
            return piece->pos;
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}


static bool is_legal(Move& psuedo_legal, Board& board) {
    const Color player_to_move = board.game_state.player_to_move;
    psuedo_legal.make_appropriate(board);
    if (is_in_check(player_to_move, board)) {
        psuedo_legal.undo_appropriate(board);
        return false;
    }
    psuedo_legal.undo_appropriate(board);
    return true;
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
    const std::vector<Move> psuedo_legal_moves = board.get_threatened_moves(opponent);
    for (const Move& move : psuedo_legal_moves) {
        if (move.end == king_square) {
            return true;
        }
    }
    return false;
}
