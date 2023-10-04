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
#include "pieces/queen.h"
#include "pieces/rook.h"

bool is_checkmate(const Board& board, const std::vector<Move>& move_history) {
   if (!is_check(board)) {
       return false;
   }

   const std::vector<Move> moves = get_all_legal_moves(board, move_history);
   for (int i = 0; i < moves.size(); i++) {
       if (!moves.at(i).leaves_king_in_check(board, move_history)) {
           return false;
       }
   }
   return true;
}

static bool is_insufficient_material(Color color, const Board& board) {
    const std::vector<std::reference_wrapper<const std::unique_ptr<Piece>>>& pieces = get_all_pieces(color, board);

    if (pieces.size() > 2) {
        return false;
    }

    for (int i = 0; i < pieces.size(); i++) {
        const std::unique_ptr<Piece>& piece = pieces.at(i);
        if (dynamic_cast<Pawn*>(piece.get()) != nullptr || dynamic_cast<Rook*>(piece.get()) != nullptr || dynamic_cast<Queen*>(piece.get()) != nullptr) {
            return false;
        }
    }
    return true;
}

static bool is_insufficient_material(const Board& board) {
    return is_insufficient_material(WHITE, board) || 
           is_insufficient_material(BLACK, board);
}

static bool is_stalemate(const Board& board, const std::vector<Move>& move_history) {
    const std::vector<Move> legal_moves = get_all_legal_moves(board, move_history);
    if (!is_check(board) && legal_moves.size() == 0) {
        return true;
    }
    return false;
}

bool is_draw(const Board& board, const std::vector<Move>& move_history) {
    return is_insufficient_material(board) || is_stalemate(board, move_history);
}
