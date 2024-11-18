#include "move_gen_lookup_tables.hpp"

struct bbs {
  u_int64_t a_file;
  u_int64_t b_file;
  u_int64_t g_file;
  u_int64_t h_file;

  u_int64_t rank_1;
  u_int64_t rank_2;
  u_int64_t rank_7;
  u_int64_t rank_8;
};

static std::array<u_int64_t, 64> create_bb_squares() {
  std::array<u_int64_t, 64> bb_squares;
  for (u_int64_t i = 0; i < 64; i++) {
    bb_squares[i] = (u_int64_t) 1 << i;
  }
  return bb_squares;
}

static u_int64_t create_bb_a_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 0; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_b_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 1; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_g_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 6; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_h_file(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 7; i < 64; i+= 8) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_1(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 0; i < 8; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_2(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 8; i < 16; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_7(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 48; i < 56; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_rank_8(std::array<u_int64_t, 64> bb_squares) {
  u_int64_t bb;
  for (int i = 56; i < 64; i++) {
    bb |= bb_squares[i];
  }
  return bb;
}

static u_int64_t create_bb_king_moves(u_int64_t king, struct bbs bbs) {
  u_int64_t bb; 
  bb |= (king & ~bbs.a_file) >> 1;
  bb |= (king & ~bbs.h_file) << 1;
  bb |= (king & ~bbs.rank_1) >> 8;
  bb |= (king & ~bbs.rank_8) << 8;

  bb |= (king & ~(bbs.a_file | bbs.rank_1)) >> 9;
  bb |= (king & ~(bbs.a_file | bbs.rank_8)) << 7;
  bb |= (king & ~(bbs.h_file | bbs.rank_1)) >> 7;
  bb |= (king & ~(bbs.h_file | bbs.rank_8)) << 9;

  return bb;
}

static u_int64_t create_bb_knight_moves(u_int64_t knight, struct bbs bbs) {
  u_int64_t bb = 0; 

  bb |= (knight & ~(bbs.a_file | bbs.rank_1 | bbs.rank_2)) >> 17;
  bb |= (knight & ~(bbs.a_file | bbs.rank_8 | bbs.rank_7)) << 15;
  bb |= (knight & ~(bbs.a_file | bbs.b_file | bbs.rank_1)) >> 10;
  bb |= (knight & ~(bbs.a_file | bbs.b_file | bbs.rank_8)) << 6;

  bb |= (knight & ~(bbs.h_file | bbs.rank_1 | bbs.rank_2)) >> 15;
  bb |= (knight & ~(bbs.h_file | bbs.rank_8 | bbs.rank_7)) << 17;
  bb |= (knight & ~(bbs.h_file | bbs.g_file | bbs.rank_1)) >> 6;
  bb |= (knight & ~(bbs.h_file | bbs.g_file | bbs.rank_8)) << 10;

  return bb;
}

MoveGenLookupTables create_lookup_tables() {
  std::array<u_int64_t, 64> bb_squares = create_bb_squares();

  struct bbs bbs = {
    .a_file = create_bb_a_file(bb_squares),
    .b_file = create_bb_b_file(bb_squares),
    .g_file = create_bb_g_file(bb_squares),
    .h_file = create_bb_h_file(bb_squares),

    .rank_1 = create_bb_rank_1(bb_squares),
    .rank_2 = create_bb_rank_2(bb_squares),
    .rank_7 = create_bb_rank_7(bb_squares),
    .rank_8 = create_bb_rank_8(bb_squares),
  };

  std::array<u_int64_t, 64> bb_knight_moves;
  std::array<u_int64_t, 64> bb_king_moves;
  for (int i = 0; i < 64; i++) {
    u_int64_t bb_square = bb_squares[i];
    bb_knight_moves[i] = create_bb_knight_moves(bb_square, bbs);
    bb_king_moves[i] = create_bb_king_moves(bb_square, bbs);
  }

  return {
    .bb_squares = bb_squares,
    .bb_knight_moves = bb_knight_moves,
    .bb_king_moves = bb_king_moves,
  };
}
