#include "bitboards_board.hpp"
#include "bits.hpp"
#include "defs.hpp"
#include "evaluation/evaluation.hpp"
#include "fmt/core.h"
#include "move.hpp"
#include "utils.hpp"
#include <cassert>
#include <optional>

BitboardsBoard::BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                               std::array<Castling, 2> castling_rights,
                               std::optional<int> en_passant_square,
                               int halfmove_clock, int fullmove_number) {

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

      PieceType piece_type = (PieceType) piece;  
      u_int64_t piece_bb = piece_bbs.at(color).at(piece);
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

  this->pos_data = {
      .player_to_move = player_to_move,
      .castling_rights = castling_rights,
      .en_passant_square = en_passant_square,
      .halfmove_clock = halfmove_clock,
      .fullmove_number = fullmove_number,
      .captured_piece = std::nullopt,
      .material = material,
      .psqt = psqt,
  };

  this->history = {};
  this->masks = create_masks();
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

std::optional<PieceType> BitboardsBoard::piece_type(int pos) const {
  std::optional<BitboardIndex> bitboard = find_bitboard_with_piece(pos);
  return bitboard.has_value()
             ? std::optional<PieceType>(bitboard.value().piece_type)
             : std::nullopt;
}

Color BitboardsBoard::player_to_move() const {
  return pos_data.player_to_move;
}

int BitboardsBoard::halfmove_clock() const {
  return pos_data.halfmove_clock;
}
int BitboardsBoard::fullmove_number() const {
  return pos_data.fullmove_number;
}
std::optional<int> BitboardsBoard::en_passant_square() const {
  return pos_data.en_passant_square;
}
std::optional<Piece> BitboardsBoard::captured_piece() const {
  return pos_data.captured_piece;
}

int BitboardsBoard::material(Color color) const {
  return pos_data.material.at(color);
}

int BitboardsBoard::psqt(Color color) const {
  return pos_data.psqt.at(color);
}

bool BitboardsBoard::is_lone_king(Color color) const {
  return bits::nr_bits_set(side_bbs.at(color)) == 1;
}

bool BitboardsBoard::is_endgame() const {
  return material(WHITE) - KING_VALUE < 1500 &&
         material(BLACK) - KING_VALUE < 1500;
}

std::optional<PieceType> BitboardsBoard::get_piece_on_pos(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      u_int64_t bitboard_bit =
          bits::get(piece_bbs.at(color).at(piece), pos);
      if (bitboard_bit == 1) {
        return (PieceType)piece;
      }
    }
  }
  return std::nullopt;
}
std::string BitboardsBoard::to_string() const {
  std::string board = "";
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      const int pos = row * 8 + col;
      std::optional<PieceType> piece = get_piece_on_pos(pos);
      char ch =
          piece.has_value() ? get_char_representation(piece.value()) : '_';
      board += fmt::format("{} ", ch);
    }
    board += '\n';
  }

  return board;
}

std::optional<BitboardIndex>
BitboardsBoard::find_bitboard_with_piece(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      u_int64_t bitboard = piece_bbs.at(color).at(piece);
      u_int64_t bitboard_bit = bits::get(bitboard, pos);
      if (bitboard_bit == 1) {
        BitboardIndex bitboard_index = {
            .color = (Color)color,
            .piece_type = (PieceType)piece,
        };
        return std::optional<BitboardIndex>(bitboard_index);
      }
    }
  }
  return std::nullopt;
}

std::optional<PieceType>
BitboardsBoard::find_piece_on_pos(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      u_int64_t bitboard = piece_bbs.at(color).at(piece);
      u_int64_t bitboard_bit = bits::get(bitboard, pos);
      if (bitboard_bit == 1) {
        return std::optional<PieceType>((PieceType) piece);
      }
    }
  }
  return std::nullopt;
}

std::array<Castling, 2>
BitboardsBoard::updated_castling_rights(const Move &move) const {
  int kingside_rook;
  int queenside_rook;
  int king;
  int opponent_kingside_rook;
  int opponent_queenside_rook;
  if (pos_data.player_to_move == WHITE) {
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
  bool disable_kingside_player = move.start == kingside_rook || move.start == king;
  bool disable_queenside_player = move.start == queenside_rook || move.start == king;

  std::array<Castling, 2> castling_rights;
  castling_rights.at(pos_data.player_to_move) = {
      .kingside =
          disable_kingside_player
              ? false
              : pos_data.castling_rights.at(pos_data.player_to_move).kingside,
      .queenside =
          disable_queenside_player
              ? false
              : pos_data.castling_rights.at(pos_data.player_to_move).queenside,
  };

  Color opponent = get_opposite_color(pos_data.player_to_move);
  bool disable_kingside_opponent = move.end == opponent_kingside_rook;
  bool disable_queenside_opponent = move.end == opponent_queenside_rook;
  castling_rights.at(opponent) = {
      .kingside = disable_kingside_opponent
                      ? false
                      : pos_data.castling_rights.at(opponent).kingside,
      .queenside = disable_queenside_opponent
                      ? false
                      : pos_data.castling_rights.at(opponent).queenside,
  };

  return castling_rights;
}

int BitboardsBoard::get_castling_rook(const Move& move, Color color) const {
    int kingside = move.end > move.start;
    if (kingside) {
      return color == WHITE ? 63 : 7;
    } else {
      return color == WHITE ? 56 : 0;
    }
}

void BitboardsBoard::make(const Move &move) {
  pos_data.next_move = move;
  history.push_back(pos_data);

  std::optional<BitboardIndex> piece_bb_index =
      find_bitboard_with_piece(move.start);
  if (!piece_bb_index.has_value()) {
    throw std::invalid_argument(fmt::format("No piece on pos: {}", move.start));
  }
  const PieceType piece_type = piece_bb_index.value().piece_type;

  std::optional<Piece> captured_piece = remove_piece(move.end);

  u_int64_t &piece_bb = piece_bbs.at(piece_bb_index.value().color)
                            .at(piece_bb_index.value().piece_type);
  u_int64_t &side_bb = side_bbs.at(pos_data.player_to_move);

  bits::unset(piece_bb, move.start);
  bits::unset(side_bb, move.start);

  const int start_psqt = get_psqt_score(
      piece_type, move.start, pos_data.player_to_move, false, false);
  pos_data.psqt.at(pos_data.player_to_move) -= start_psqt;

  bits::set(side_bb, move.end);
  if (move.move_type == CASTLING) {
    int kingside = move.end > move.start;
    int rook = get_castling_rook(move, pos_data.player_to_move);
    int rook_new = kingside ? rook - 2 : rook + 3;
    u_int64_t &rook_bb = piece_bbs.at(pos_data.player_to_move).at(ROOK);
    bits::unset(rook_bb, rook);
    bits::unset(side_bb, rook);
    bits::set(rook_bb, rook_new);
    bits::set(side_bb, rook_new);
    bits::set(piece_bb, move.end);

    int start_rook_psqt =
        get_psqt_score(ROOK, rook, pos_data.player_to_move, false, false);
    int end_rook_psqt =
        get_psqt_score(ROOK, rook_new, pos_data.player_to_move, false, false);
    int end_king_psqt = get_psqt_score(KING, move.end,
                                  pos_data.player_to_move, false, false);
    pos_data.psqt.at(pos_data.player_to_move) +=
        end_king_psqt - start_rook_psqt + end_rook_psqt;
  } else if (move.move_type == PROMOTION) {
    assert(move.promotion_piece.has_value());
    u_int64_t &promotion_piece_bb =
        piece_bbs.at(pos_data.player_to_move).at(move.promotion_piece.value());
    bits::set(promotion_piece_bb, move.end);
    pos_data.material.at(player_to_move()) +=
        get_piece_value(move.promotion_piece.value()) - PAWN_VALUE;
    int end_psqt = get_psqt_score(move.promotion_piece.value(), move.end,
                                  pos_data.player_to_move, false, false);
    pos_data.psqt.at(pos_data.player_to_move) += end_psqt;
  } else if (move.move_type == EN_PASSANT) {
    assert(pos_data.en_passant_square.has_value());
    int captured_piece_square = pos_data.player_to_move == WHITE
      ? pos_data.en_passant_square.value() + 8
      : pos_data.en_passant_square.value() - 8;
    captured_piece = remove_piece(captured_piece_square);
    bits::set(piece_bb, move.end);
    int new_psqt = get_psqt_score(piece_type, move.end, pos_data.player_to_move,
                                  false, false);
    pos_data.psqt.at(pos_data.player_to_move) += new_psqt;
  } else {
    bits::set(piece_bb, move.end);
    int new_psqt = get_psqt_score(piece_type, move.end, pos_data.player_to_move,
                                  false, false);
    pos_data.psqt.at(pos_data.player_to_move) += new_psqt;
  }

  pos_data.castling_rights = updated_castling_rights(move);

  pos_data.en_passant_square =
      move.move_type == PAWN_TWO_SQUARES_FORWARD
          ? std::optional<int>((move.start + move.end) / 2)
          : std::nullopt;
  pos_data.halfmove_clock = piece_type == PAWN || captured_piece.has_value()
                                ? 0
                                : pos_data.halfmove_clock + 1;
  pos_data.fullmove_number =
      pos_data.fullmove_number + (pos_data.player_to_move == BLACK ? 1 : 0);
  pos_data.captured_piece = captured_piece;

  pos_data.player_to_move = get_opposite_color(pos_data.player_to_move);
}

std::optional<Piece> BitboardsBoard::remove_piece(int pos) {
  std::optional<BitboardIndex> piece_bb_index =
      find_bitboard_with_piece(pos);
  if (!piece_bb_index.has_value()) {
    return std::nullopt;
  }

  const Color color = piece_bb_index.value().color;
  const PieceType piece_type = piece_bb_index.value().piece_type;
  u_int64_t &piece_bb =
      piece_bbs.at(piece_bb_index.value().color)
          .at(piece_bb_index.value().piece_type);
  u_int64_t &side_bb = side_bbs.at(piece_bb_index.value().color);

  bits::unset(piece_bb, pos);
  bits::unset(side_bb, pos);

  pos_data.material.at(color) -= get_piece_value(piece_type);
  pos_data.psqt.at(color) -=
      get_psqt_score(piece_type, pos, color, false, false);

  return Piece(piece_type, color, pos);
}

void BitboardsBoard::undo() {
  assert(history.size() >= 1);

  const Move &move = history.back().next_move;
  std::optional<BitboardIndex> piece_bb_index =
      find_bitboard_with_piece(move.end);
  if (!piece_bb_index.has_value()) {
    throw std::invalid_argument(
        fmt::format("No piece on pos: {}", move.end));
  }
  u_int64_t &piece_bb = piece_bbs.at(piece_bb_index.value().color)
                            .at(piece_bb_index.value().piece_type);
  u_int64_t &side_bb = side_bbs.at(history.back().player_to_move);
  bits::unset(piece_bb, move.end);
  bits::unset(side_bb, move.end);
  bits::set(side_bb, move.start);

  if (move.move_type == CASTLING) {
    int kingside = move.end > move.start;
    int rook = get_castling_rook(move, history.back().player_to_move);
    int rook_new = kingside ? rook - 2 : rook + 3;
    u_int64_t &rook_bb = piece_bbs.at(history.back().player_to_move).at(ROOK);
    bits::unset(rook_bb, rook_new);
    bits::unset(side_bb, rook_new);
    bits::set(rook_bb, rook);
    bits::set(side_bb, rook);
    bits::set(piece_bb, move.start);
  } else if (move.move_type == PROMOTION) {
    bits::set(piece_bbs.at(history.back().player_to_move).at(PAWN), move.start);
  } else {
    bits::set(piece_bb, move.start);
  }

  if (pos_data.captured_piece.has_value()) {
    u_int64_t &bb_captured_piece =
        piece_bbs.at(pos_data.captured_piece.value().color)
            .at(pos_data.captured_piece.value().piece_type);
    bits::set(bb_captured_piece, pos_data.captured_piece.value().pos);
    bits::set(side_bbs.at(pos_data.captured_piece.value().color),
              pos_data.captured_piece.value().pos);
  }

  pos_data = history.back();
  history.pop_back();
}

// TODO: implement draws
bool BitboardsBoard::is_draw() const { return false; }
