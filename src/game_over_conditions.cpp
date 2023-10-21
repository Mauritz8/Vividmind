#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "board.h"
#include "game_over_conditions.h"
#include "move.h"
#include "game_state.h"
#include "piece.h"
#include "pieces/pawn.h"


bool is_checkmate(Board& board, std::vector<Move>& legal_moves) {
   if (!is_in_check(board.game_state.player_to_move, board)) {
       return false;
   }

   for (Move& move : legal_moves) {
       if (!move.leaves_king_in_check(board)) {
           return false;
       }
   }
   return true;
}

static bool is_insufficient_material(const std::vector<std::shared_ptr<Piece>>& pieces) {
    if (pieces.size() > 2) {
        return false;
    }

    for (auto piece : pieces) {
        if (piece->piece_type == PAWN || piece->piece_type == ROOK || piece->piece_type == QUEEN) {
            return false;
        }
    }
    return true;
}

static bool is_insufficient_material(const Board& board) {
    return is_insufficient_material(board.game_state.pieces[WHITE]) || 
           is_insufficient_material(board.game_state.pieces[BLACK]);
}

static bool is_stalemate(Board& board, const std::vector<Move>& legal_moves) {
    if (!is_in_check(board.game_state.player_to_move, board) && legal_moves.size() == 0) {
        return true;
    }
    return false;
}

bool is_draw(Board& board, std::vector<Move>& legal_moves) {
    return is_insufficient_material(board) || is_stalemate(board, legal_moves);
}
