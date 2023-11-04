#include "game_over_detector.hpp"


GameOverDetector::GameOverDetector(Board& board, const MoveGenerator& move_generator) 
    : board(board)
    , move_gen(move_generator)
{}

bool GameOverDetector::is_checkmate(std::vector<Move>& legal_moves) const {
   if (!move_gen.is_in_check(board.game_state.player_to_move)) {
       return false;
   }

   const Color player = board.game_state.player_to_move;
   for (const Move& move : legal_moves) {
       board.make(move);
       if (!move_gen.is_in_check(player)) {
           board.undo();
           return false;
       }
       board.undo();
   }
   return true;
}

bool GameOverDetector::is_insufficient_material() const {
    for (const std::vector<Piece>& pieces : board.game_state.pieces) {
        for (Piece piece : pieces) {
            if (piece.piece_type == PAWN || piece.piece_type == ROOK || piece.piece_type == QUEEN) {
                return false;
            }
        }
    }
    return true;
}

bool GameOverDetector::is_threefold_repetition() const {
    int repetitions = 1;
    for (int i = board.history.size() - 1; i >= 0; i--) {
        const GameState& old_state = board.history.at(i);
        if (old_state == board.game_state) {
            repetitions++;  
        }         
        if (repetitions == 3) {
            return true;
        }
    }
    return false;
}
