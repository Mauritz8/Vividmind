#include "search.hpp"

int Search::calc_allocated_time() const {
  const int allocated_time = board.get_player_to_move() == WHITE
                                 ? params.game_time.wtime / 20
                                 : params.game_time.btime / 20;

  return allocated_time == 0 ? 1 : allocated_time;
}
