#include "bitboards.hpp"
#include <sys/types.h>

static std::array<u_int64_t, 64> create_bb_squares() {
  std::array<u_int64_t, 64> bb_squares;
  for (u_int64_t i = 0; i < 64; i++) {
    bb_squares[i] = (u_int64_t) 1 << i;
  }
  return bb_squares;
}

static u_int64_t create_bb_a_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 0; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_b_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 1; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_g_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 6; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_h_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 7; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_1(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 0; i < 8; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_2(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 8; i < 16; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_7(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 48; i < 56; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_8(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb = 0;
  for (int i = 56; i < 64; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

const std::array<u_int64_t, 64> bb_squares = create_bb_squares();
const u_int64_t a_file = create_bb_a_file(bb_squares);
const u_int64_t b_file = create_bb_b_file(bb_squares);
const u_int64_t g_file = create_bb_g_file(bb_squares);
const u_int64_t h_file = create_bb_h_file(bb_squares);

const u_int64_t rank_1 = create_bb_rank_1(bb_squares);
const u_int64_t rank_2 = create_bb_rank_2(bb_squares);
const u_int64_t rank_7 = create_bb_rank_7(bb_squares);
const u_int64_t rank_8 = create_bb_rank_8(bb_squares);

static u_int64_t create_bb_king_moves(u_int64_t king) {
  u_int64_t bb = 0; 
  bb |= (king & ~a_file) >> 1;
  bb |= (king & ~h_file) << 1;
  bb |= (king & ~rank_1) >> 8;
  bb |= (king & ~rank_8) << 8;

  bb |= (king & ~(a_file | rank_1)) >> 9;
  bb |= (king & ~(a_file | rank_8)) << 7;
  bb |= (king & ~(h_file | rank_1)) >> 7;
  bb |= (king & ~(h_file | rank_8)) << 9;

  return bb;
}

static u_int64_t create_bb_knight_moves(u_int64_t knight) {
  u_int64_t bb = 0; 

  bb |= (knight & ~(a_file | rank_1 | rank_2)) >> 17;
  bb |= (knight & ~(a_file | rank_8 | rank_7)) << 15;
  bb |= (knight & ~(a_file | b_file | rank_1)) >> 10;
  bb |= (knight & ~(a_file | b_file | rank_8)) << 6;

  bb |= (knight & ~(h_file | rank_1 | rank_2)) >> 15;
  bb |= (knight & ~(h_file | rank_8 | rank_7)) << 17;
  bb |= (knight & ~(h_file | g_file | rank_1)) >> 6;
  bb |= (knight & ~(h_file | g_file | rank_8)) << 10;

  return bb;
}

static u_int64_t create_bb_white_pawn_moves_straight(u_int64_t pawn) {
  u_int64_t bb = 0;
  bb |= (pawn & ~rank_1) >> 8;
  bb |= (pawn & rank_7) >> 16;
  return bb;
}

static u_int64_t create_bb_black_pawn_moves_straight(u_int64_t pawn) {
  u_int64_t bb = 0;
  bb |= (pawn & ~rank_8) << 8;
  bb |= (pawn & rank_2) << 16;
  return bb;
}

Bitboards create_bitboards() {
  std::array<u_int64_t, 64> bb_knight_moves;
  std::array<u_int64_t, 64> bb_king_moves;
  std::array<u_int64_t, 64> bb_white_pawn_moves_straight;
  std::array<u_int64_t, 64> bb_black_pawn_moves_straight;
  for (int i = 0; i < 64; i++) {
    u_int64_t bb_square = bb_squares[i];
    bb_knight_moves[i] = create_bb_knight_moves(bb_square);
    bb_king_moves[i] = create_bb_king_moves(bb_square);
    bb_white_pawn_moves_straight[i] =
        create_bb_white_pawn_moves_straight(bb_square);
    bb_black_pawn_moves_straight[i] =
        create_bb_black_pawn_moves_straight(bb_square);
  }

  return {.squares = bb_squares,
          .knight_moves = bb_knight_moves,
          .king_moves = bb_king_moves,
          .pawn_moves_straight = {bb_white_pawn_moves_straight,
                                  bb_black_pawn_moves_straight}};
}
