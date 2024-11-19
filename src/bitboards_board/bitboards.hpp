#include <array>
#include <sys/types.h>

struct Bitboards {
  std::array<u_int64_t, 64> squares;
  std::array<u_int64_t, 64> knight_moves;
  std::array<u_int64_t, 64> king_moves;
  std::array<std::array<u_int64_t, 64>, 2> pawn_moves_straight;
  std::array<std::array<u_int64_t, 64>, 2> pawn_captures;
  std::array<u_int64_t, 64> rook_moves;
  u_int64_t a_file;
  u_int64_t h_file;
  u_int64_t rank_1;
  u_int64_t rank_8;
};

Bitboards create_bitboards();
