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

bool is_checkmate(const Board& board) {
   if (!is_in_check(board.player_to_move, board)) {
       return false;
   }

   const std::vector<Move> moves = get_all_legal_moves(board);
   for (int i = 0; i < moves.size(); i++) {
       if (!moves.at(i).leaves_king_in_check(board)) {
           return false;
       }
   }
   return true;
}

static bool is_insufficient_material(Color color, const Board& board) {
    auto pieces = get_all_pieces(color, board);

    if (pieces.size() > 2) {
        return false;
    }

    for (int i = 0; i < pieces.size(); i++) {
        const std::shared_ptr<Piece>& piece = pieces.at(i);
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

static bool is_stalemate(const Board& board) {
    const std::vector<Move> legal_moves = get_all_legal_moves(board);
    if (!is_in_check(board.player_to_move, board) && legal_moves.size() == 0) {
        return true;
    }
    return false;
}

bool is_draw(const Board& board) {
    return is_insufficient_material(board) || is_stalemate(board);
}
