#include "bitboards.hpp"
#include <sys/types.h>

static std::array<u_int64_t, 64> create_bb_squares() {
  std::array<u_int64_t, 64> bb_squares;
  for (u_int64_t i = 0; i < 64; i++) {
    bb_squares[i] = (u_int64_t) 1 << i;
  }
  return bb_squares;
}

static std::array<u_int64_t, 8> create_rank_masks(std::array<u_int64_t, 64> bb_squares) {
  std::array<u_int64_t, 8> rank_masks;
  for (int i = 0; i < 64; i++) {
    int rank = i / 8;
    rank_masks.at(rank) |= bb_squares[i];
  }
  return rank_masks;
}

static std::array<u_int64_t, 8> create_file_masks(std::array<u_int64_t, 64> bb_squares) {
  std::array<u_int64_t, 8> file_masks;
  for (int i = 0; i < 64; i++) {
    int file = i % 8;
    file_masks.at(file) |= bb_squares[i];
  }
  return file_masks;
}

const std::array<u_int64_t, 64> bb_squares = create_bb_squares();
const std::array<u_int64_t, 8> files = create_file_masks(bb_squares);
const std::array<u_int64_t, 8> ranks = create_rank_masks(bb_squares);

static u_int64_t create_bb_king_moves(u_int64_t king) {
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

static u_int64_t create_bb_knight_moves(u_int64_t knight) {
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

static u_int64_t create_bb_white_pawn_moves_one(u_int64_t pawn) {
  return (pawn & ~ranks.at(0)) >> 8;
}

static u_int64_t create_bb_white_pawn_moves_two(u_int64_t pawn) {
  return (pawn & ranks.at(6)) >> 16; 
}

static u_int64_t create_bb_black_pawn_moves_one(u_int64_t pawn) {
  return (pawn & ~ranks.at(7)) << 8;
}

static u_int64_t create_bb_black_pawn_moves_two(u_int64_t pawn) {
  return (pawn & ranks.at(1)) << 16; 
}

static u_int64_t create_bb_white_pawn_captures(u_int64_t pawn) {
  u_int64_t bb = 0;
  bb |= (pawn & ~(ranks.at(0) | files.at(7))) >> 7;
  bb |= (pawn & ~(ranks.at(0) | files.at(0))) >> 9;
  return bb;
}

static u_int64_t create_bb_black_pawn_captures(u_int64_t pawn) {
  u_int64_t bb = 0;
  bb |= (pawn & ~(ranks.at(7) | files.at(0))) << 7;
  bb |= (pawn & ~(ranks.at(7) | files.at(7))) << 9;
  return bb;
}

static u_int64_t create_bb_rook_moves(u_int64_t rook) {
  u_int64_t bb = 0;

  for (u_int64_t rank : ranks) {
    if ((rook & rank) != 0) {
      bb |= rank & ~rook;
      break;
    }
  }

  for (u_int64_t file : files) {
    if ((rook & file) != 0) {
      bb |= file & ~rook;
      break;
    }
  }

  return bb;
}

Bitboards create_bitboards() {
  std::array<u_int64_t, 64> bb_knight_moves;
  std::array<u_int64_t, 64> bb_king_moves;
  std::array<u_int64_t, 64> bb_white_pawn_moves_one;
  std::array<u_int64_t, 64> bb_white_pawn_moves_two;
  std::array<u_int64_t, 64> bb_black_pawn_moves_one;
  std::array<u_int64_t, 64> bb_black_pawn_moves_two;
  std::array<u_int64_t, 64> bb_white_pawn_captures;
  std::array<u_int64_t, 64> bb_black_pawn_captures;
  std::array<u_int64_t, 64> bb_rook_moves;
  for (int i = 0; i < 64; i++) {
    u_int64_t bb_square = bb_squares[i];
    bb_knight_moves[i] = create_bb_knight_moves(bb_square);
    bb_king_moves[i] = create_bb_king_moves(bb_square);
    bb_white_pawn_moves_one[i] = create_bb_white_pawn_moves_one(bb_square);
    bb_white_pawn_moves_two[i] = create_bb_white_pawn_moves_two(bb_square);
    bb_black_pawn_moves_one[i] = create_bb_black_pawn_moves_one(bb_square);
    bb_black_pawn_moves_two[i] = create_bb_black_pawn_moves_two(bb_square);
    bb_white_pawn_captures[i] = create_bb_white_pawn_captures(bb_square);
    bb_black_pawn_captures[i] = create_bb_black_pawn_captures(bb_square);
    bb_rook_moves[i] = create_bb_rook_moves(bb_square);
  }

  return {
      .squares = bb_squares,
      .knight_moves = bb_knight_moves,
      .king_moves = bb_king_moves,
      .pawn_moves_one = {bb_white_pawn_moves_one, bb_black_pawn_moves_one},
      .pawn_moves_two = {bb_white_pawn_moves_two, bb_black_pawn_moves_two},
      .pawn_captures = {bb_white_pawn_captures, bb_black_pawn_captures},
      .rook_moves = bb_rook_moves,
      .files = files,
      .ranks = ranks,
  };
}
