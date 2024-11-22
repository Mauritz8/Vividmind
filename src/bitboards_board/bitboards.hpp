#include <array>
#include <sys/types.h>

struct Bitboards {
  std::array<u_int64_t, 64> squares;
  std::array<u_int64_t, 64> knight_moves;
  std::array<u_int64_t, 64> king_moves;
  std::array<std::array<u_int64_t, 64>, 2> pawn_moves_one;
  std::array<std::array<u_int64_t, 64>, 2> pawn_moves_two;
  std::array<std::array<u_int64_t, 64>, 2> pawn_captures;
  std::array<u_int64_t, 64> rook_moves;
  std::array<u_int64_t, 8> files;
  std::array<u_int64_t, 8> ranks;
};

Bitboards create_bitboards();
