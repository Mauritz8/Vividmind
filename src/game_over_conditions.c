#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "game_over_conditions.h"
#include "move.h"
#include "game_state.h"
#include "piece.h"

bool is_checkmate(Board* board, const MoveArray* move_history) {
   if (!is_check(board)) {
       return false;
   }

   MoveArray moves = get_all_legal_moves(board, move_history);
   for (int i = 0; i < moves.length; i++) {
       if (!leaves_king_in_check(&moves.moves[i], board, move_history)) {
           free(moves.moves);
           return false;
       }
   }
   free(moves.moves);
   return true;
}

static bool is_insufficient_material(Board* board) {
    const PieceArray white_pieces = get_all_pieces(WHITE, board);
    const PieceArray black_pieces = get_all_pieces(BLACK, board);

    if (white_pieces.length > 2 || black_pieces.length > 2) {
        return false;
    }

    for (int i = 0; i < white_pieces.length; i++) {
        const Piece piece = white_pieces.pieces[i];
        if (piece.piece_type == PAWN || piece.piece_type == ROOK || piece.piece_type == QUEEN) {
            return false;
        }
    }
    for (int i = 0; i < black_pieces.length; i++) {
        const Piece piece = black_pieces.pieces[i];
        if (piece.piece_type == PAWN || piece.piece_type == ROOK || piece.piece_type == QUEEN) {
            return false;
        }
    }
    return true;
}

static bool is_stalemate(Board* board, const MoveArray* move_history) {
    MoveArray legal_moves = get_all_legal_moves(board, move_history);
    if (!is_check(board) && legal_moves.length == 0) {
        return true;
    }
    return false;
}

bool is_draw(Board* board, const MoveArray* move_history) {
    return is_insufficient_material(board) || is_stalemate(board, move_history);
}
