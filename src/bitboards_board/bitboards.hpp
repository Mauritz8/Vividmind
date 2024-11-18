#include <array>
#include <sys/types.h>

struct Bitboards {
  std::array<u_int64_t, 64> squares;
  std::array<u_int64_t, 64> knight_moves;
  std::array<u_int64_t, 64> king_moves;
  std::array<std::array<u_int64_t, 64>, 2> pawn_moves_straight;
};

Bitboards create_bitboards();
