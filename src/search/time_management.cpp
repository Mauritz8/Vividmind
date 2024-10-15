#include "time_management.hpp"

int calc_allocated_time(Color player_to_move, int white_remaining_time,
                        int black_remaining_time) {
  const int allocated_time = player_to_move == WHITE
                                 ? white_remaining_time / 20
                                 : black_remaining_time / 20;

  return allocated_time == 0 ? 1 : allocated_time;
}
