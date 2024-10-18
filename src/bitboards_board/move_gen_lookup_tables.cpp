#include "move_gen_lookup_tables.hpp"
#include "bitboards_board/bits.hpp"
#include "fmt/core.h"

static std::array<u_int64_t, 64> create_bb_squares() {
  std::array<u_int64_t, 64> bb_squares;
  for (u_int64_t i = 0; i < 64; i++) {
    bb_squares[i] = (u_int64_t) 1 << i;
  }
  return bb_squares;
}

static u_int64_t create_bb_knight_moves(int square, u_int64_t bb_square) {
  u_int64_t up_right = square > 15 && square % 8 != 7 ? bb_square >> 15 : 0;
  u_int64_t right_up =
      square > 7 && square % 8 != 7 && square % 8 != 6 ? bb_square >> 6 : 0;
  u_int64_t right_down =
      square < 56 && square % 8 != 7 && square % 8 != 6 ? bb_square << 10 : 0;
  u_int64_t down_right = square < 48 && square % 8 != 7 ? bb_square << 17 : 0;
  u_int64_t down_left = square < 48 && square % 8 != 0 ? bb_square << 15 : 0;
  u_int64_t left_down = square < 56 && square % 8 != 1 ? bb_square << 6 : 0;
  u_int64_t left_up =
      square > 7 && square % 8 != 0 && square % 8 != 1 ? bb_square >> 10 : 0;
  u_int64_t up_left = square > 15 && square % 8 != 0 ? bb_square >> 17 : 0;

  return up_right | right_up | right_down | down_right | down_left | left_down |
         left_up | up_left;
}

MoveGenLookupTables create_lookup_tables() {
  std::array<u_int64_t, 64> bb_knight_moves;

  std::array<u_int64_t, 64> bb_squares = create_bb_squares();
  for (int i = 0; i < 64; i++) {
    u_int64_t bb_square = bb_squares[i];
    bb_knight_moves[i] = create_bb_knight_moves(i, bb_square);
  }
  return {
    .bb_squares = bb_squares,
    .bb_knight_moves = bb_knight_moves,
  };
}
