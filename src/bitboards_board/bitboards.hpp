#include <array>
#include <sys/types.h>

struct Bitboards {
  std::array<u_int64_t, 64> squares;
  std::array<u_int64_t, 64> knight_moves;
  std::array<u_int64_t, 64> king_moves;
};

Bitboards create_bitboards();
