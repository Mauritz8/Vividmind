#include "bitboards_board.hpp"
#include "bitboards_board/bits.hpp"
#include "defs.hpp"
#include "fmt/core.h"
#include "move.hpp"
#include "utils.hpp"
#include <cassert>
#include <sys/types.h>

u_int64_t BitboardsBoard::get_castling_check_not_allowed_bb(int start,
                                                   bool kingside) const {
  u_int64_t bb = masks.squares.at(start);
  return bb |= kingside ? masks.squares.at(start + 1) | masks.squares.at(start + 2)
    : masks.squares.at(start - 1) | masks.squares.at(start - 2);
}

u_int64_t
BitboardsBoard::get_castling_pieces_not_allowed_bb(int start,
                                                   bool kingside) const {
  return kingside ? masks.squares.at(start + 1) | masks.squares.at(start + 2)
                  : masks.squares.at(start - 1) | masks.squares.at(start - 2) |
                        masks.squares.at(start - 3);
}

u_int64_t BitboardsBoard::gen_castling_moves_bb(int start) const {
  u_int64_t castling = 0;

  int king_initial = pos_data.player_to_move == WHITE ? 60 : 4;
  if (start != king_initial) {
    return 0;
  }

  if (is_in_check(pos_data.player_to_move)) {
    return 0;
  }

  Castling castling_rights =
      pos_data.castling_rights.at(pos_data.player_to_move);

  u_int64_t attacked_bb =
      castling_rights.kingside || castling_rights.queenside
          ? get_attacking_bb(get_opposite_color(pos_data.player_to_move))
          : 0;

  u_int64_t pieces_bb =
      (side_bbs.at(pos_data.player_to_move) &
       ~piece_bbs.at(pos_data.player_to_move).at(KING)) |
      side_bbs.at(get_opposite_color(pos_data.player_to_move));

  if (castling_rights.kingside) {
    u_int64_t no_check_bb = get_castling_check_not_allowed_bb(start, true);
    u_int64_t no_pieces_bb = get_castling_pieces_not_allowed_bb(start, true);
    if (((no_check_bb & attacked_bb) | (no_pieces_bb & pieces_bb)) == 0) {
      castling |= masks.squares.at(start + 2);
    }
  }

  if (castling_rights.queenside) {
    u_int64_t no_check_bb = get_castling_check_not_allowed_bb(start, false);
    u_int64_t no_pieces_bb = get_castling_pieces_not_allowed_bb(start, false);
    if (((no_check_bb & attacked_bb) | (no_pieces_bb & pieces_bb)) == 0) {
      castling |= masks.squares.at(start - 2);
    }
  }

  return castling;
}

std::vector<Move> BitboardsBoard::gen_king_moves(int start) const {
  std::vector<Move> moves;

  u_int64_t normal =
      masks.king_moves.at(start) & ~side_bbs.at(pos_data.player_to_move);

  std::optional<int> end_pos = bits::popLSB(normal);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(normal);
  }

  u_int64_t castling = gen_castling_moves_bb(start);
  end_pos = bits::popLSB(castling);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    move.move_type = CASTLING;
    moves.push_back(move);
    end_pos = bits::popLSB(castling);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_pawn_moves(int start) const {
  std::vector<Move> moves;

  u_int64_t pawn = masks.squares.at(start);
  u_int64_t all_pieces = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  u_int64_t all_pieces_one_rank_forward =
      pos_data.player_to_move == WHITE ? all_pieces >> 8 : all_pieces << 8;

  u_int64_t move_one = masks.pawn_moves_one.at(pos_data.player_to_move).at(start)
    & ~all_pieces;
  u_int64_t move_two = masks.pawn_moves_two.at(pos_data.player_to_move).at(start)
    & ~(all_pieces | all_pieces_one_rank_forward);

  u_int64_t captures = masks.pawn_captures.at(pos_data.player_to_move).at(start)
    & ~side_bbs.at(pos_data.player_to_move);
  u_int64_t normal_captures =
      captures & side_bbs.at(get_opposite_color(pos_data.player_to_move));

  u_int64_t en_passant_captures = pos_data.en_passant_square.has_value()
    ? captures & masks.squares.at(pos_data.en_passant_square.value())
    : 0;

  u_int64_t bb_end = move_one | normal_captures;
  std::optional<int> end_pos = bits::popLSB(bb_end);
  while (end_pos.has_value()) {
    if (end_pos.value() < 8 || end_pos.value() > 55) {
      std::array<PieceType, 4> promotion_pieces = {
          QUEEN,
          ROOK,
          BISHOP,
          KNIGHT,
      };
      for (PieceType p : promotion_pieces) {
        Move move = Move(start, end_pos.value());
        move.move_type = PROMOTION;
        move.promotion_piece = p;
        moves.push_back(move);
      }
    } else {
      Move move = Move(start, end_pos.value());
      moves.push_back(move);
    }
    end_pos = bits::popLSB(bb_end);
  }

  end_pos = bits::popLSB(move_two);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    move.move_type = PAWN_TWO_SQUARES_FORWARD;
    moves.push_back(move);
    end_pos = bits::popLSB(move_two);
  }

  end_pos = bits::popLSB(en_passant_captures);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    move.move_type = EN_PASSANT;
    moves.push_back(move);
    end_pos = bits::popLSB(en_passant_captures);
  }

  return moves;
}

u_int64_t BitboardsBoard::gen_sliding_attacks(int start, u_int64_t occupied,
                                            u_int64_t mask) const {
  u_int64_t forward = 0;
  u_int64_t reverse = 0;
  forward = occupied & mask;
  reverse = bits::reverse(forward);
  forward -= 2 * masks.squares.at(start);
  reverse -= 2 * bits::reverse(masks.squares.at(start));
  forward ^= bits::reverse(reverse);
  forward &= mask;
  return forward;
}

u_int64_t BitboardsBoard::gen_file_attacks(int start, u_int64_t occupied) const {
  int file = start % 8;
  return gen_sliding_attacks(start, occupied, masks.files.at(file));
}

u_int64_t BitboardsBoard::gen_rank_attacks(int start, u_int64_t occupied) const {
  int rank = start / 8;
  return gen_sliding_attacks(start, occupied, masks.ranks.at(rank));
}

u_int64_t BitboardsBoard::gen_diag_attacks(int start,
                                             u_int64_t occupied) const {
  int file = start % 8;
  int rank = start / 8;
  int diag = file + rank;
  return gen_sliding_attacks(start, occupied, masks.diags.at(diag));
}

u_int64_t BitboardsBoard::gen_antidiag_attacks(int start,
                                                  u_int64_t occupied) const {
  int file = start % 8;
  int rank = start / 8;
  int diag = file - rank + 7;
  return gen_sliding_attacks(start, occupied, masks.antidiags.at(diag));
  return 0;
}

u_int64_t BitboardsBoard::gen_rook_attacks(int start, u_int64_t occupied) const {
  return gen_file_attacks(start, occupied) | gen_rank_attacks(start, occupied);
}

u_int64_t BitboardsBoard::gen_bishop_attacks(int start,
                                           u_int64_t occupied) const {
  return gen_diag_attacks(start, occupied) |
         gen_antidiag_attacks(start, occupied);
}

u_int64_t BitboardsBoard::gen_queen_attacks(int start, u_int64_t occupied) const {
  return gen_rook_attacks(start, occupied) | gen_bishop_attacks(start, occupied);
}

std::vector<Move> BitboardsBoard::gen_moves_piece(PieceType piece, int start) const {
  if (piece == KING) {
    return gen_king_moves(start);
  }
  if (piece == PAWN) {
    return gen_pawn_moves(start);
  }

  u_int64_t occupied = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  u_int64_t attacks = piece == KNIGHT   ? masks.knight_moves.at(start)
                       : piece == BISHOP ? gen_bishop_attacks(start, occupied)
                       : piece == ROOK   ? gen_rook_attacks(start, occupied)
                                         : gen_queen_attacks(start, occupied);
  attacks &= ~side_bbs.at(pos_data.player_to_move);

  std::vector<Move> moves;
  std::optional<int> end_pos = bits::popLSB(attacks);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(attacks);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_all_moves_piece(PieceType piece) const {
  std::vector<Move> moves;

  u_int64_t piece_bb = piece_bbs.at(pos_data.player_to_move).at(piece);
  std::optional<int> start_pos = bits::popLSB(piece_bb);
  while (start_pos.has_value()) {
    std::vector<Move> pos_moves = gen_moves_piece(piece, start_pos.value());
    moves.insert(moves.end(), pos_moves.begin(), pos_moves.end()); 
    start_pos = bits::popLSB(piece_bb);
  }
  return moves;
}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
  std::vector<Move> moves;
  for (int piece = 0; piece < 6; piece++) {
    std::vector<Move> piece_moves = gen_all_moves_piece((PieceType) piece);
    moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
  }
  return moves;
}

u_int64_t BitboardsBoard::get_attacking_bb(Color color) const {
  u_int64_t attacking = 0;

  std::array<PieceType, 3> non_sliding_pieces = {KING, PAWN, KNIGHT};
  for (PieceType piece : non_sliding_pieces) {
    std::array<u_int64_t, 64> piece_attacking_bb =
        piece == KING   ? masks.king_moves
        : piece == PAWN ? masks.pawn_captures.at(color)
                        : masks.knight_moves;
    u_int64_t piece_bb = piece_bbs.at(color).at(piece);
    std::optional<int> start_pos = bits::popLSB(piece_bb);
    while (start_pos.has_value()) {
      attacking |= piece_attacking_bb.at(start_pos.value());
      start_pos = bits::popLSB(piece_bb);
    }
  }

  u_int64_t occupied = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  std::array<PieceType, 3> sliding_pieces = {BISHOP, ROOK, QUEEN};
  for (PieceType piece : sliding_pieces) {
    u_int64_t piece_bb = piece_bbs.at(color).at(piece);
    std::optional<int> start_pos = bits::popLSB(piece_bb);
    while (start_pos.has_value()) {
      attacking |=
          piece == BISHOP ? gen_bishop_attacks(start_pos.value(), occupied)
          : piece == ROOK ? gen_rook_attacks(start_pos.value(), occupied)
                          : gen_queen_attacks(start_pos.value(), occupied);

      start_pos = bits::popLSB(piece_bb);
    }
  }

  return attacking;
}

bool BitboardsBoard::is_attacking(int pos, Color color) const {
  std::array<u_int64_t, 6> pieces_bb = piece_bbs.at(color);

  if ((masks.knight_moves.at(pos) & pieces_bb.at(KNIGHT)) != 0) {
    return true;
  }
  if ((masks.king_moves.at(pos) & pieces_bb.at(KING)) != 0) {
    return true;
  }
  if ((masks.pawn_captures.at(get_opposite_color(color)).at(pos) &
       pieces_bb.at(PAWN)) != 0) {
    return true;
  }

  u_int64_t occupied = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  u_int64_t bishop_moves = gen_bishop_attacks(pos, occupied);
  if ((bishop_moves & pieces_bb.at(BISHOP)) != 0) {
    return true;
  }
  u_int64_t rook_moves = gen_rook_attacks(pos, occupied);
  if ((rook_moves & pieces_bb.at(ROOK)) != 0) {
    return true;
  }
  if (((bishop_moves | rook_moves) & pieces_bb.at(QUEEN)) != 0) {
    return true;
  }

  return false;
}

bool BitboardsBoard::is_in_check(Color color) const {
  u_int64_t king_bb = piece_bbs.at(color).at(KING);
  assert(king_bb != 0);
  int king_pos = bits::popLSB(king_bb).value();
  return is_attacking(king_pos, get_opposite_color(color));
}
