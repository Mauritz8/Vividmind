#include <stdlib.h>

#include "engine/engine.h"
#include "game_state.h"
#include "move.h"

static int get_random_num(int min, int max) {
    return min + rand() % (max - min);
}

Move get_best_move(const Color player_to_move, Board* board, MoveArray* move_history) {
    MoveArray legal_moves = get_all_legal_moves(player_to_move, board, move_history);
    int random_index = get_random_num(0, legal_moves.length);
    return legal_moves.moves[random_index];
}
