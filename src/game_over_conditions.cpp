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

static bool is_insufficient_material(const Board& board) {
    for (const std::vector<Piece>& pieces : board.game_state.pieces) {
        for (Piece piece : pieces) {
            if (piece.piece_type == PAWN || piece.piece_type == ROOK || piece.piece_type == QUEEN) {
                return false;
            }
        }
    }
    return true;
}

static bool is_stalemate(Board& board, const std::vector<Move>& legal_moves) {
    if (legal_moves.size() == 0 && !is_in_check(board.game_state.player_to_move, board)) {
        return true;
    }
    return false;
}

static bool is_threefold_repetition(Board& board) {
    int repetitions = 1;
    for (int i = board.history.size() - 1; i >= 0; i--) {
        GameState& old_state = board.history.at(i);
        if (old_state == board.game_state) {
            repetitions++;  
        }         
        if (repetitions == 3) {
            return true;
        }
    }
    return false;
}

bool is_draw(Board& board, std::vector<Move>& legal_moves) {
    return is_insufficient_material(board)
        || is_stalemate(board, legal_moves)
        || is_threefold_repetition(board);
}
