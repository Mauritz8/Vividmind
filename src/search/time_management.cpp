#include "search.hpp"


int Search::calc_allocated_time() {
    if (board.game_state.player_to_move == WHITE) {
        return params.game_time.wtime / 20;
    }
    return params.game_time.btime / 20;
}
