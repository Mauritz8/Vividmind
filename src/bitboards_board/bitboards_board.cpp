#include "bitboards_board.hpp"
#include "bits.hpp"
#include "defs.hpp"
#include "evaluation/evaluation.hpp"
#include "fmt/core.h"
#include "move.hpp"
#include "utils.hpp"
#include <cassert>
#include <optional>
#include <stdint.h>

BitboardsBoard::BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                               std::array<Castling, 2> castling_rights,
                               std::optional<int> en_passant_square,
                               int halfmove_clock, int fullmove_number)
    : masks(create_masks()) {

  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      piece_bbs.at(color).at(piece) = 0;
    }
  }
  for (const Piece &piece : pieces) {
    bits::set(piece_bbs.at(piece.color).at(piece.piece_type), piece.pos);
  }

  std::array<int, 2> material;
  std::array<int, 2> psqt;
  for (int color = 0; color < 2; color++) {
    side_bbs.at(color) = 0;
    int material_side = 0;
    int psqt_side = 0;
    for (int piece = 0; piece < 6; piece++) {
      side_bbs.at(color) |= piece_bbs.at(color).at(piece);

      PieceType piece_type = (PieceType)piece;
      uint64_t piece_bb = piece_bbs.at(color).at(piece);
      material_side +=
          bits::nr_bits_set(piece_bb) * get_piece_value(piece_type);

      std::optional<int> pos = bits::popLSB(piece_bb);
      while (pos.has_value()) {
        psqt_side +=
            get_psqt_score(piece_type, pos.value(), (Color)color, false, false);
        pos = bits::popLSB(piece_bb);
      }
    }
    material.at(color) = material_side;
    psqt.at(color) = psqt_side;
  }

  std::stack<PosData> history;
  PosData pos_data = {
      .player_to_move = player_to_move,
      .castling_rights = castling_rights,
      .en_passant_square = en_passant_square,
      .halfmove_clock = halfmove_clock,
      .fullmove_number = fullmove_number,
      .captured_piece = std::nullopt,
      .material = material,
      .psqt = psqt,
  };
  history.push(pos_data);
  this->history = history;

  std::stack<Move> moves;
  this->move_history = moves;
}

bool BitboardsBoard::operator==(const BitboardsBoard &other) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      if (this->piece_bbs.at(color).at(piece) !=
          other.piece_bbs.at(color).at(piece)) {
        return false;
      }
    }
  }
  return true;
}

std::optional<PieceType> BitboardsBoard::get_piece_type(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      uint64_t bitboard_bit = bits::get(piece_bbs.at(color).at(piece), pos);
      if (bitboard_bit == 1) {
        return (PieceType)piece;
      }
    }
  }
  return std::nullopt;
}

Color BitboardsBoard::get_player_to_move() const {
  return history.top().player_to_move;
}

int BitboardsBoard::get_halfmove_clock() const {
  return history.top().halfmove_clock;
}
int BitboardsBoard::get_fullmove_number() const {
  return history.top().fullmove_number;
}
std::optional<int> BitboardsBoard::get_en_passant_square() const {
  return history.top().en_passant_square;
}
std::optional<Piece> BitboardsBoard::get_captured_piece() const {
  return history.top().captured_piece;
}

int BitboardsBoard::get_material(Color color) const {
  return history.top().material.at(color);
}

int BitboardsBoard::get_psqt(Color color) const {
  return history.top().psqt.at(color);
}

bool BitboardsBoard::is_lone_king(Color color) const {
  return bits::nr_bits_set(side_bbs.at(color)) == 1;
}

bool BitboardsBoard::is_endgame() const {
  return get_material(WHITE) - KING_VALUE < 1500 &&
         get_material(BLACK) - KING_VALUE < 1500;
}

std::string BitboardsBoard::to_string() const {
  std::string board = "";
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      const int pos = row * 8 + col;
      std::optional<PieceType> piece = get_piece_type(pos);
      char ch =
          piece.has_value() ? get_char_representation(piece.value()) : '_';
      board += fmt::format("{} ", ch);
    }
    board += '\n';
  }

  return board;
}

std::array<Castling, 2>
BitboardsBoard::updated_castling_rights(const Move &move) const {
  int kingside_rook;
  int queenside_rook;
  int king;
  int opponent_kingside_rook;
  int opponent_queenside_rook;
  if (get_player_to_move() == WHITE) {
    kingside_rook = 63;
    queenside_rook = 56;
    king = 60;
    opponent_kingside_rook = 7;
    opponent_queenside_rook = 0;
  } else {
    kingside_rook = 7;
    queenside_rook = 0;
    king = 4;
    opponent_kingside_rook = 63;
    opponent_queenside_rook = 56;
  }
  bool disable_kingside_player =
      move.start == kingside_rook || move.start == king;
  bool disable_queenside_player =
      move.start == queenside_rook || move.start == king;

  std::array<Castling, 2> castling_rights;
  castling_rights.at(get_player_to_move()) = {
      .kingside =
          disable_kingside_player
              ? false
              : history.top().castling_rights.at(get_player_to_move()).kingside,
      .queenside = disable_queenside_player
                       ? false
                       : history.top()
                             .castling_rights.at(get_player_to_move())
                             .queenside,
  };

  Color opponent = get_opposite_color(get_player_to_move());
  bool disable_kingside_opponent = move.end == opponent_kingside_rook;
  bool disable_queenside_opponent = move.end == opponent_queenside_rook;
  castling_rights.at(opponent) = {
      .kingside = disable_kingside_opponent
                      ? false
                      : history.top().castling_rights.at(opponent).kingside,
      .queenside = disable_queenside_opponent
                       ? false
                       : history.top().castling_rights.at(opponent).queenside,
  };

  return castling_rights;
}

int BitboardsBoard::get_castling_rook(const Move &move, Color color) const {
  int kingside = move.end > move.start;
  if (kingside) {
    return color == WHITE ? 63 : 7;
  } else {
    return color == WHITE ? 56 : 0;
  }
}

std::optional<PieceType> BitboardsBoard::piece_type(int pos,
                                                    Color color) const {
  std::array<uint64_t, NR_PIECES> side_bb = piece_bbs.at(color);
  for (int piece = 0; piece < NR_PIECES; piece++) {
    uint64_t piece_bb = side_bb.at(piece);
    if (bits::get(piece_bb, pos) == 1) {
      return (PieceType)piece;
    }
  }
  return std::nullopt;
}

std::optional<Piece>
BitboardsBoard::get_piece_to_be_captured(const Move &move) const {
  Color player = get_player_to_move();
  Color opponent = get_opposite_color(player);
  int pos = move.move_type == EN_PASSANT
                ? get_en_passant_square().value() + (player == WHITE ? 8 : -8)
                : move.end;
  std::optional<PieceType> piece_type_opt = piece_type(pos, opponent);
  return piece_type_opt.has_value()
             ? std::optional<Piece>(
                   Piece(piece_type_opt.value(), opponent, pos))
             : std::nullopt;
}

std::array<int, 2>
BitboardsBoard::updated_material(const Move &move,
                                 std::optional<Piece> captured_piece) const {
  const Color player_to_move = get_player_to_move();
  const Color opponent = get_opposite_color(player_to_move);

  std::array<int, 2> material;
  material.at(player_to_move) =
      get_material(player_to_move) +
      (move.move_type == PROMOTION
           ? get_piece_value(move.promotion_piece.value()) -
                 get_piece_value(PAWN)
           : 0);
  material.at(opponent) =
      get_material(opponent) -
      (captured_piece.has_value()
           ? get_piece_value(captured_piece.value().piece_type)
           : 0);
  return material;
}

std::array<int, 2>
BitboardsBoard::updated_psqt(const Move &move,
                             std::optional<Piece> captured_piece) const {
  const Color player_to_move = get_player_to_move();
  const Color opponent = get_opposite_color(player_to_move);

  const std::optional<PieceType> piece_type_optional =
      piece_type(move.start, player_to_move);
  assert(piece_type_optional.has_value());
  const PieceType piece_type = piece_type_optional.value();
  const PieceType new_piece_type =
      move.move_type == PROMOTION ? move.promotion_piece.value() : piece_type;

  bool endgame = piece_type == PieceType::KING ? is_endgame() : false;
  bool lone_king =
      piece_type == PieceType::KING ? is_lone_king(player_to_move) : false;
  std::array<int, 2> psqt;
  psqt.at(player_to_move) = get_psqt(player_to_move) -
                            get_psqt_score(piece_type, move.start,
                                           player_to_move, lone_king, endgame) +
                            get_psqt_score(new_piece_type, move.end,
                                           player_to_move, lone_king, endgame);
  if (move.move_type == CASTLING) {
    const int kingside = move.end > move.start;
    const int rook_start = get_castling_rook(move, player_to_move);
    const int rook_end = rook_start + (kingside ? -2 : 3);
    psqt.at(player_to_move) +=
        get_psqt_score(ROOK, rook_end, player_to_move, false, false) -
        get_psqt_score(ROOK, rook_start, player_to_move, false, false);
  }

  psqt.at(opponent) =
      get_psqt(opponent) -
      (captured_piece.has_value()
           ? get_psqt_score(captured_piece.value().piece_type,
                            captured_piece.value().pos,
                            captured_piece.value().color, false, false)
           : 0);
  return psqt;
}

void BitboardsBoard::make(const Move &move) {

  const Color player_to_move = get_player_to_move();
  const std::optional<PieceType> piece_type_opt =
      piece_type(move.start, player_to_move);
  assert(piece_type_opt.has_value());
  const PieceType piece_type = piece_type_opt.value();

  const std::optional<Piece> captured_piece_opt =
      get_piece_to_be_captured(move);

  const PosData new_pos_data = {
      .player_to_move = get_opposite_color(player_to_move),
      .castling_rights = updated_castling_rights(move),
      .en_passant_square = move.move_type == PAWN_TWO_SQUARES_FORWARD
                               ? std::optional<int>((move.start + move.end) / 2)
                               : std::nullopt,
      .halfmove_clock = piece_type == PAWN || captured_piece_opt.has_value()
                            ? 0
                            : history.top().halfmove_clock + 1,
      .fullmove_number =
          history.top().fullmove_number + (player_to_move == BLACK ? 1 : 0),
      .captured_piece = captured_piece_opt,
      .material = updated_material(move, captured_piece_opt),
      .psqt = updated_psqt(move, captured_piece_opt),
  };

  history.push(new_pos_data);
  move_history.push(move);

  uint64_t &piece_bb = piece_bbs.at(player_to_move).at(piece_type);
  uint64_t &side_bb = side_bbs.at(player_to_move);

  bits::unset(piece_bb, move.start);
  bits::unset(side_bb, move.start);
  bits::set(side_bb, move.end);
  if (move.move_type == CASTLING) {
    const int kingside = move.end > move.start;
    const int rook = get_castling_rook(move, player_to_move);
    const int rook_new = kingside ? rook - 2 : rook + 3;
    uint64_t &rook_bb = piece_bbs.at(player_to_move).at(ROOK);
    bits::unset(rook_bb, rook);
    bits::unset(side_bb, rook);
    bits::set(rook_bb, rook_new);
    bits::set(side_bb, rook_new);
    bits::set(piece_bb, move.end);
  } else if (move.move_type == PROMOTION) {
    assert(move.promotion_piece.has_value());
    uint64_t &promotion_piece_bb =
        piece_bbs.at(player_to_move).at(move.promotion_piece.value());
    bits::set(promotion_piece_bb, move.end);
  } else {
    bits::set(piece_bb, move.end);
  }

  if (captured_piece_opt.has_value()) {
    const Piece p = captured_piece_opt.value();
    bits::unset(piece_bbs.at(p.color).at(p.piece_type), p.pos);
    bits::unset(side_bbs.at(p.color), p.pos);
  }
}

void BitboardsBoard::undo() {
  assert(move_history.size() >= 1);

  const Move &move = move_history.top();

  const Color move_played_by = get_opposite_color(get_player_to_move());
  const std::optional<PieceType> piece_type_opt =
      piece_type(move.end, move_played_by);
  assert(piece_type_opt.has_value());
  const PieceType piece_type = piece_type_opt.value();

  uint64_t &piece_bb = piece_bbs.at(move_played_by).at(piece_type);
  uint64_t &side_bb = side_bbs.at(move_played_by);

  bits::unset(piece_bb, move.end);
  bits::unset(side_bb, move.end);
  bits::set(side_bb, move.start);

  if (move.move_type == CASTLING) {
    int kingside = move.end > move.start;
    int rook = get_castling_rook(move, move_played_by);
    int rook_new = kingside ? rook - 2 : rook + 3;
    uint64_t &rook_bb = piece_bbs.at(move_played_by).at(ROOK);
    bits::unset(rook_bb, rook_new);
    bits::unset(side_bb, rook_new);
    bits::set(rook_bb, rook);
    bits::set(side_bb, rook);
    bits::set(piece_bb, move.start);
  } else if (move.move_type == PROMOTION) {
    bits::set(piece_bbs.at(move_played_by).at(PAWN), move.start);
  } else {
    bits::set(piece_bb, move.start);
  }

  const std::optional<Piece> captured_piece_opt = history.top().captured_piece;
  if (captured_piece_opt.has_value()) {
    const Piece p = captured_piece_opt.value();
    bits::set(piece_bbs.at(p.color).at(p.piece_type), p.pos);
    bits::set(side_bbs.at(p.color), p.pos);
  }

  history.pop();
  move_history.pop();
}

bool BitboardsBoard::is_insufficient_material() const {
  uint64_t all_pieces_bb = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  if (bits::nr_bits_set(all_pieces_bb) > 3) {
    return false;
  }

  std::array<PieceType, 3> pieces = {PAWN, ROOK, QUEEN};
  for (PieceType p : pieces) {
    for (int color = 0; color < 2; color++) {
      if (piece_bbs.at(color).at(p) != 0) {
        return false;
      }
    }
  }
  return true;
}

bool BitboardsBoard::is_draw_by_fifty_move_rule() const {
  return history.top().halfmove_clock > 100;
}

bool BitboardsBoard::is_threefold_repetition() const {
  if (history.top().halfmove_clock < 5) {
    return false;
  }

  BitboardsBoard b = *this;
  while (!(b.move_history.empty() || history.top().halfmove_clock == 0)) {
    b.undo();
    if (*this == b) {
      return true;
    }
  }
  return false;
}

int BitboardsBoard::get_doubled_pawns(Color color) const {
  uint64_t pawn_bb = piece_bbs.at(color).at(PieceType::PAWN);
  int doubled_pawns = 0;
  for (int i = 0; i < 8; i++) {
    uint64_t pawns_file = pawn_bb & masks.files.at(i);
    if (bits::nr_bits_set(pawns_file) > 1) {
      doubled_pawns++;
    }
  }
  return doubled_pawns;
}
