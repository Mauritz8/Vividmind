#include "masks.hpp"
#include "bitboards_board/bits.hpp"
#include "fmt/core.h"
#include <sys/types.h>

static std::array<u_int64_t, 64> square_masks() {
  std::array<u_int64_t, 64> square_masks;
  for (u_int64_t i = 0; i < 64; i++) {
    square_masks[i] = (u_int64_t) 1 << i;
  }
  return square_masks;
}

const std::array<u_int64_t, 64> squares = square_masks();

static std::array<u_int64_t, 8> rank_masks() {
  std::array<u_int64_t, 8> masks;
  std::fill(std::begin(masks), std::end(masks), 0);
  for (int i = 0; i < 64; i++) {
    int rank = i / 8;
    masks.at(rank) |= squares[i];
  }
  return masks;
}

static std::array<u_int64_t, 8> file_masks() {
  std::array<u_int64_t, 8> masks;
  std::fill(std::begin(masks), std::end(masks), 0);
  for (int i = 0; i < 64; i++) {
    int file = i % 8;
    masks.at(file) |= squares[i];
  }
  return masks;
}

static std::array<u_int64_t, 15> diag_masks() {
  std::array<u_int64_t, 15> masks;
  std::fill(std::begin(masks), std::end(masks), 0);
  for (int i = 0; i < 64; i++) {
    int file = i % 8;
    int rank = i / 8;
    int diag = file + rank;
    masks.at(diag) |= squares[i];
  }
  return masks;
}

static std::array<u_int64_t, 15> antidiag_masks() {
  std::array<u_int64_t, 15> masks;
  std::fill(std::begin(masks), std::end(masks), 0);
  for (int i = 0; i < 64; i++) {
    int file = i % 8;
    int rank = i / 8;
    int diag = file - rank + 7;
    masks.at(diag) |= squares[i];
  }
  return masks;
}

const std::array<u_int64_t, 8> files = file_masks();
const std::array<u_int64_t, 8> ranks = rank_masks();

static u_int64_t king_moves_mask(u_int64_t king) {
  u_int64_t bb = 0; 
  bb |= (king & ~files.at(0)) >> 1;
  bb |= (king & ~files.at(7)) << 1;
  bb |= (king & ~ranks.at(0)) >> 8;
  bb |= (king & ~ranks.at(7)) << 8;

  bb |= (king & ~(files.at(0) | ranks.at(0))) >> 9;
  bb |= (king & ~(files.at(0) | ranks.at(7))) << 7;
  bb |= (king & ~(files.at(7) | ranks.at(0))) >> 7;
  bb |= (king & ~(files.at(7) | ranks.at(7))) << 9;

  return bb;
}

static u_int64_t knight_moves_mask(u_int64_t knight) {
  u_int64_t bb = 0; 

  bb |= (knight & ~(files.at(0) | ranks.at(0) | ranks.at(1))) >> 17;
  bb |= (knight & ~(files.at(0) | ranks.at(7) | ranks.at(6))) << 15;
  bb |= (knight & ~(files.at(0) | files.at(1) | ranks.at(0))) >> 10;
  bb |= (knight & ~(files.at(0) | files.at(1) | ranks.at(7))) << 6;

  bb |= (knight & ~(files.at(7) | ranks.at(0) | ranks.at(1))) >> 15;
  bb |= (knight & ~(files.at(7) | ranks.at(7) | ranks.at(6))) << 17;
  bb |= (knight & ~(files.at(7) | files.at(6) | ranks.at(0))) >> 6;
  bb |= (knight & ~(files.at(7) | files.at(6) | ranks.at(7))) << 10;

  return bb;
}

static u_int64_t white_pawn_moves_one_mask(u_int64_t pawn) {
  return (pawn & ~ranks.at(0)) >> 8;
}

static u_int64_t white_pawn_moves_two_mask(u_int64_t pawn) {
  return (pawn & ranks.at(6)) >> 16; 
}

static u_int64_t black_pawn_moves_one_mask(u_int64_t pawn) {
  return (pawn & ~ranks.at(7)) << 8;
}

static u_int64_t black_pawn_moves_two_mask(u_int64_t pawn) {
  return (pawn & ranks.at(1)) << 16; 
}

static u_int64_t white_pawn_captures_mask(u_int64_t pawn) {
  u_int64_t bb = 0;
  bb |= (pawn & ~(ranks.at(0) | files.at(7))) >> 7;
  bb |= (pawn & ~(ranks.at(0) | files.at(0))) >> 9;
  return bb;
}

static u_int64_t black_pawn_captures_mask(u_int64_t pawn) {
  u_int64_t bb = 0;
  bb |= (pawn & ~(ranks.at(7) | files.at(0))) << 7;
  bb |= (pawn & ~(ranks.at(7) | files.at(7))) << 9;
  return bb;
}

Masks create_masks() {
  std::array<u_int64_t, 64> knight_moves_masks;
  std::array<u_int64_t, 64> king_moves_masks;
  std::array<u_int64_t, 64> white_pawn_moves_one_masks;
  std::array<u_int64_t, 64> white_pawn_moves_two_masks;
  std::array<u_int64_t, 64> black_pawn_moves_one_masks;
  std::array<u_int64_t, 64> black_pawn_moves_two_masks;
  std::array<u_int64_t, 64> white_pawn_captures_masks;
  std::array<u_int64_t, 64> black_pawn_captures_masks;
  for (int i = 0; i < 64; i++) {
    u_int64_t bb_square = squares[i];
    knight_moves_masks[i] = knight_moves_mask(bb_square);
    king_moves_masks[i] = king_moves_mask(bb_square);
    white_pawn_moves_one_masks[i] = white_pawn_moves_one_mask(bb_square);
    white_pawn_moves_two_masks[i] = white_pawn_moves_two_mask(bb_square);
    black_pawn_moves_one_masks[i] = black_pawn_moves_one_mask(bb_square);
    black_pawn_moves_two_masks[i] = black_pawn_moves_two_mask(bb_square);
    white_pawn_captures_masks[i] = white_pawn_captures_mask(bb_square);
    black_pawn_captures_masks[i] = black_pawn_captures_mask(bb_square);
  }

  return {
      .squares = squares,
      .knight_moves = knight_moves_masks,
      .king_moves = king_moves_masks,
      .pawn_moves_one = {white_pawn_moves_one_masks, black_pawn_moves_one_masks},
      .pawn_moves_two = {white_pawn_moves_two_masks, black_pawn_moves_two_masks},
      .pawn_captures = {white_pawn_captures_masks, black_pawn_captures_masks},
      .files = files,
      .ranks = ranks,
      .diags = diag_masks(),
      .antidiags = antidiag_masks(),
  };
}
