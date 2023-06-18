#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "move.h"
#include "game_state.h"

bool is_checkmated(const Color color, Board* board) {
   if (!is_in_check(color, board)) {
       return false;
   }

   MoveArray moves = get_all_legal_moves(color, board);
   for (int i = 0; i < moves.length; i++) {
       if (!leaves_king_in_check(&moves.moves[i], board)) {
           free(moves.moves);
           return false;
       }
   }
   free(moves.moves);
   return true;
}
