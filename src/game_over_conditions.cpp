#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>

#include "board.h"
#include "game_over_conditions.h"
#include "move.h"
#include "game_state.h"
#include "piece.h"

bool is_checkmate(const Board& board, std::vector<Move> move_history) {
   if (!is_check(board)) {
       return false;
   }

   std::vector<Move> moves = get_all_legal_moves(board, move_history);
   for (int i = 0; i < moves.size(); i++) {
       if (!moves.at(i).leaves_king_in_check(board, move_history)) {
           return false;
       }
   }
   return true;
}

static bool is_insufficient_material(const Board& board) {
    const std::vector<Piece> white_pieces = get_all_pieces(WHITE, board);
    const std::vector<Piece> black_pieces = get_all_pieces(BLACK, board);

    if (white_pieces.size() > 2 || black_pieces.size() > 2) {
        return false;
    }

    for (int i = 0; i < white_pieces.size(); i++) {
        Piece piece = white_pieces.at(i);
        if (piece.get_piece_type() == PAWN || piece.get_piece_type() == ROOK || piece.get_piece_type() == QUEEN) {
            return false;
        }
    }
    for (int i = 0; i < black_pieces.size(); i++) {
        Piece piece = black_pieces.at(i);
        if (piece.get_piece_type() == PAWN || piece.get_piece_type() == ROOK || piece.get_piece_type() == QUEEN) {
            return false;
        }
    }
    return true;
}

static bool is_stalemate(const Board& board, const std::vector<Move> move_history) {
    std::vector<Move> legal_moves = get_all_legal_moves(board, move_history);
    if (!is_check(board) && legal_moves.size() == 0) {
        return true;
    }
    return false;
}

bool is_draw(const Board& board, const std::vector<Move> move_history) {
    return is_insufficient_material(board) || is_stalemate(board, move_history);
}
