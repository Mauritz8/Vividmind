#pragma once

#include <array>
#include <sys/types.h>
#include <stdint.h>

struct Masks {
  std::array<uint64_t, 64> squares;
  std::array<uint64_t, 64> knight_moves;
  std::array<uint64_t, 64> king_moves;
  std::array<std::array<uint64_t, 64>, 2> pawn_moves_one;
  std::array<std::array<uint64_t, 64>, 2> pawn_moves_two;
  std::array<std::array<uint64_t, 64>, 2> pawn_captures;
  std::array<uint64_t, 8> files;
  std::array<uint64_t, 8> ranks;
  std::array<uint64_t, 15> diags;
  std::array<uint64_t, 15> antidiags;
};

Masks create_masks();
