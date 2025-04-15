#include "bitboards_board.hpp"
#include "bitboards_board/bits.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "utils.hpp"
#include <cassert>

uint64_t
BitboardsBoard::get_castling_check_not_allowed_bb(int start,
                                                  bool kingside) const {
  uint64_t bb = masks.squares.at(start);
  return bb |= kingside
                   ? masks.squares.at(start + 1) | masks.squares.at(start + 2)
                   : masks.squares.at(start - 1) | masks.squares.at(start - 2);
}

uint64_t
BitboardsBoard::get_castling_pieces_not_allowed_bb(int start,
                                                   bool kingside) const {
  return kingside ? masks.squares.at(start + 1) | masks.squares.at(start + 2)
                  : masks.squares.at(start - 1) | masks.squares.at(start - 2) |
                        masks.squares.at(start - 3);
}

uint64_t BitboardsBoard::gen_castling_moves_bb(int start) const {
  uint64_t castling = 0;

  int king_initial = get_player_to_move() == WHITE ? 60 : 4;
  if (start != king_initial) {
    return 0;
  }

  if (is_in_check(get_player_to_move())) {
    return 0;
  }

  Castling castling_rights =
      history.top().castling_rights.at(get_player_to_move());

  uint64_t attacked_bb =
      castling_rights.kingside || castling_rights.queenside
          ? get_attacking_bb(get_opposite_color(get_player_to_move()))
          : 0;

  uint64_t pieces_bb = (side_bbs.at(get_player_to_move()) &
                        ~piece_bbs.at(get_player_to_move()).at(KING)) |
                       side_bbs.at(get_opposite_color(get_player_to_move()));

  if (castling_rights.kingside) {
    uint64_t no_check_bb = get_castling_check_not_allowed_bb(start, true);
    uint64_t no_pieces_bb = get_castling_pieces_not_allowed_bb(start, true);
    if (((no_check_bb & attacked_bb) | (no_pieces_bb & pieces_bb)) == 0) {
      castling |= masks.squares.at(start + 2);
    }
  }

  if (castling_rights.queenside) {
    uint64_t no_check_bb = get_castling_check_not_allowed_bb(start, false);
    uint64_t no_pieces_bb = get_castling_pieces_not_allowed_bb(start, false);
    if (((no_check_bb & attacked_bb) | (no_pieces_bb & pieces_bb)) == 0) {
      castling |= masks.squares.at(start - 2);
    }
  }

  return castling;
}

void BitboardsBoard::gen_king_moves(int start, MoveCategory move_category,
                                    std::vector<Move> &moves) const {
  uint64_t normal = masks.king_moves.at(start);
  normal &= move_category == TACTICAL
                ? side_bbs.at(get_opposite_color(get_player_to_move()))
                : ~side_bbs.at(get_player_to_move());

  std::optional<int> end_pos = bits::popLSB(normal);
  while (end_pos.has_value()) {
    Move move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(normal);
  }

  if (move_category == ALL) {
    uint64_t castling = gen_castling_moves_bb(start);
    end_pos = bits::popLSB(castling);
    while (end_pos.has_value()) {
      Move move(start, end_pos.value(), CASTLING);
      moves.push_back(move);
      end_pos = bits::popLSB(castling);
    }
  }
}

void BitboardsBoard::gen_pawn_moves(int start, MoveCategory move_category,
                                    std::vector<Move> &moves) const {
  uint64_t pawn = masks.squares.at(start);
  uint64_t all_pieces = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  uint64_t all_pieces_one_rank_forward =
      get_player_to_move() == WHITE ? all_pieces >> 8 : all_pieces << 8;

  uint64_t move_one =
      masks.pawn_moves_one.at(get_player_to_move()).at(start) & ~all_pieces;
  uint64_t move_two = masks.pawn_moves_two.at(get_player_to_move()).at(start) &
                      ~(all_pieces | all_pieces_one_rank_forward);

  uint64_t captures = masks.pawn_captures.at(get_player_to_move()).at(start) &
                      ~side_bbs.at(get_player_to_move());
  uint64_t normal_captures =
      captures & side_bbs.at(get_opposite_color(get_player_to_move()));

  uint64_t en_passant_captures =
      get_en_passant_square().has_value()
          ? captures & masks.squares.at(get_en_passant_square().value())
          : 0;

  std::optional<int> end_pos = bits::popLSB(move_one);
  while (end_pos.has_value()) {
    bool is_promotion = end_pos.value() < 8 || end_pos.value() > 55;
    if (is_promotion) {
      std::array<PieceType, 4> promotion_pieces = {
          QUEEN,
          ROOK,
          BISHOP,
          KNIGHT,
      };
      for (PieceType p : promotion_pieces) {
        Move move(start, end_pos.value(), p);
        moves.push_back(move);
      }
    } else if (move_category == ALL) {
      Move move(start, end_pos.value());
      moves.push_back(move);
    }
    end_pos = bits::popLSB(move_one);
  }

  end_pos = bits::popLSB(normal_captures);
  while (end_pos.has_value()) {
    bool is_promotion = end_pos.value() < 8 || end_pos.value() > 55;
    if (is_promotion) {
      std::array<PieceType, 4> promotion_pieces = {
          QUEEN,
          ROOK,
          BISHOP,
          KNIGHT,
      };
      for (PieceType p : promotion_pieces) {
        Move move(start, end_pos.value(), p);
        moves.push_back(move);
      }
    } else {
      Move move(start, end_pos.value());
      moves.push_back(move);
    }
    end_pos = bits::popLSB(normal_captures);
  }

  if (move_category == ALL) {
    end_pos = bits::popLSB(move_two);
    while (end_pos.has_value()) {
      Move move(start, end_pos.value(), PAWN_TWO_SQUARES_FORWARD);
      moves.push_back(move);
      end_pos = bits::popLSB(move_two);
    }
  }

  end_pos = bits::popLSB(en_passant_captures);
  while (end_pos.has_value()) {
    Move move(start, end_pos.value(), EN_PASSANT);
    moves.push_back(move);
    end_pos = bits::popLSB(en_passant_captures);
  }
}

uint64_t BitboardsBoard::gen_sliding_attacks(int start, uint64_t occupied,
                                             uint64_t mask) const {
  uint64_t forward = 0;
  uint64_t reverse = 0;
  forward = occupied & mask;
  reverse = bits::reverse(forward);
  forward -= 2 * masks.squares.at(start);
  reverse -= 2 * bits::reverse(masks.squares.at(start));
  forward ^= bits::reverse(reverse);
  forward &= mask;
  return forward;
}

uint64_t BitboardsBoard::gen_file_attacks(int start, uint64_t occupied) const {
  int file = start % 8;
  return gen_sliding_attacks(start, occupied, masks.files.at(file));
}

uint64_t BitboardsBoard::gen_rank_attacks(int start, uint64_t occupied) const {
  int rank = start / 8;
  return gen_sliding_attacks(start, occupied, masks.ranks.at(rank));
}

uint64_t BitboardsBoard::gen_diag_attacks(int start, uint64_t occupied) const {
  int file = start % 8;
  int rank = start / 8;
  int diag = file + rank;
  return gen_sliding_attacks(start, occupied, masks.diags.at(diag));
}

uint64_t BitboardsBoard::gen_antidiag_attacks(int start,
                                              uint64_t occupied) const {
  int file = start % 8;
  int rank = start / 8;
  int diag = file - rank + 7;
  return gen_sliding_attacks(start, occupied, masks.antidiags.at(diag));
}

uint64_t BitboardsBoard::gen_rook_attacks(int start, uint64_t occupied) const {
  return gen_file_attacks(start, occupied) | gen_rank_attacks(start, occupied);
}

uint64_t BitboardsBoard::gen_bishop_attacks(int start,
                                            uint64_t occupied) const {
  return gen_diag_attacks(start, occupied) |
         gen_antidiag_attacks(start, occupied);
}

uint64_t BitboardsBoard::gen_queen_attacks(int start, uint64_t occupied) const {
  return gen_rook_attacks(start, occupied) |
         gen_bishop_attacks(start, occupied);
}

void BitboardsBoard::gen_moves_piece(PieceType piece, int start,
                                     MoveCategory move_category,
                                     std::vector<Move> &moves) const {
  if (piece == KING) {
    gen_king_moves(start, move_category, moves);
    return;
  }
  if (piece == PAWN) {
    gen_pawn_moves(start, move_category, moves);
    return;
  }

  uint64_t occupied = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  uint64_t attacks = piece == KNIGHT   ? masks.knight_moves.at(start)
                     : piece == BISHOP ? gen_bishop_attacks(start, occupied)
                     : piece == ROOK   ? gen_rook_attacks(start, occupied)
                                       : gen_queen_attacks(start, occupied);
  uint64_t moves_bb =
      move_category == TACTICAL
          ? attacks & side_bbs.at(get_opposite_color(get_player_to_move()))
          : attacks &= ~side_bbs.at(get_player_to_move());

  std::optional<int> end_pos = bits::popLSB(moves_bb);
  while (end_pos.has_value()) {
    Move move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(moves_bb);
  }
}

void BitboardsBoard::gen_all_moves_piece(PieceType piece,
                                         MoveCategory move_category,
                                         std::vector<Move> &moves) const {
  uint64_t piece_bb = piece_bbs.at(get_player_to_move()).at(piece);
  std::optional<int> start_pos = bits::popLSB(piece_bb);
  while (start_pos.has_value()) {
    gen_moves_piece(piece, start_pos.value(), move_category, moves);
    start_pos = bits::popLSB(piece_bb);
  }
}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
  std::vector<Move> moves;
  for (int piece = 0; piece < 6; piece++) {
    gen_all_moves_piece((PieceType)piece, move_category, moves);
  }
  return moves;
}

uint64_t BitboardsBoard::get_attacking_bb(Color color) const {
  uint64_t attacking = 0;

  std::array<PieceType, 3> non_sliding_pieces = {KING, PAWN, KNIGHT};
  for (PieceType piece : non_sliding_pieces) {
    std::array<uint64_t, 64> piece_attacking_bb =
        piece == KING   ? masks.king_moves
        : piece == PAWN ? masks.pawn_captures.at(color)
                        : masks.knight_moves;
    uint64_t piece_bb = piece_bbs.at(color).at(piece);
    std::optional<int> start_pos = bits::popLSB(piece_bb);
    while (start_pos.has_value()) {
      attacking |= piece_attacking_bb.at(start_pos.value());
      start_pos = bits::popLSB(piece_bb);
    }
  }

  uint64_t occupied = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  std::array<PieceType, 3> sliding_pieces = {BISHOP, ROOK, QUEEN};
  for (PieceType piece : sliding_pieces) {
    uint64_t piece_bb = piece_bbs.at(color).at(piece);
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
  std::array<uint64_t, 6> pieces_bb = piece_bbs.at(color);

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

  uint64_t occupied = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  uint64_t bishop_moves = gen_bishop_attacks(pos, occupied);
  if ((bishop_moves & pieces_bb.at(BISHOP)) != 0) {
    return true;
  }
  uint64_t rook_moves = gen_rook_attacks(pos, occupied);
  if ((rook_moves & pieces_bb.at(ROOK)) != 0) {
    return true;
  }
  if (((bishop_moves | rook_moves) & pieces_bb.at(QUEEN)) != 0) {
    return true;
  }

  return false;
}

bool BitboardsBoard::is_in_check(Color color) const {
  uint64_t king_bb = piece_bbs.at(color).at(KING);
  assert(king_bb != 0);
  int king_pos = bits::popLSB(king_bb).value();
  return is_attacking(king_pos, get_opposite_color(color));
}
