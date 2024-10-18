#include "bitboards_board.hpp"
#include "bits.hpp"
#include "defs.hpp"
#include "fmt/core.h"
#include "utils.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <sys/types.h>

BitboardsBoard::BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                               std::array<Castling, 2> castling_rights,
                               std::optional<int> en_passant_square,
                               int halfmove_clock, int fullmove_number) {

  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      bb_pieces.at(color).at(piece) = 0;
    }
  }
  for (const Piece &piece : pieces) {
    bits::set(bb_pieces.at(piece.color).at(piece.piece_type), piece.pos);
  }

  this->pos_data = {
      .player_to_move = player_to_move,
      .castling_rights = castling_rights,
      .en_passant_square = en_passant_square,
      .halfmove_clock = halfmove_clock,
      .fullmove_number = fullmove_number,
      .captured_piece = std::nullopt,
  };

  this->history = {};
  this->move_gen_lookup_tables = create_lookup_tables();
}

bool BitboardsBoard::operator==(const BitboardsBoard &other) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      if (this->bb_pieces.at(color).at(piece) !=
          other.bb_pieces.at(color).at(piece)) {
        return false;
      }
    }
  }
  return true;
}

std::optional<PieceType> BitboardsBoard::get_piece_type(int pos) const {
  std::optional<BitboardIndex> bitboard = find_bitboard_with_piece(pos);
  return bitboard.has_value()
             ? std::optional<PieceType>(bitboard.value().piece_type)
             : std::nullopt;
}

Color BitboardsBoard::get_player_to_move() const {
  return pos_data.player_to_move;
}

int BitboardsBoard::get_material(Color color) const { return 0; }

int BitboardsBoard::get_psqt(Color color) const { return 0; }

std::optional<PieceType> BitboardsBoard::get_piece_on_pos(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      u_int64_t bitboard_bit =
          bits::get(bb_pieces.at(color).at(piece), pos);
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
      u_int64_t bitboard = bb_pieces.at(color).at(piece);
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

void BitboardsBoard::make(const Move &move) {
  pos_data.next_move = move;
  history.push_back(pos_data);

  std::optional<BitboardIndex> piece_bitboard_index =
      find_bitboard_with_piece(move.start);
  if (!piece_bitboard_index.has_value()) {
    throw std::invalid_argument(fmt::format("No piece on pos: {}", move.start));
  }
  u_int64_t &piece_bitboard =
      bb_pieces.at(piece_bitboard_index.value().color)
          .at(piece_bitboard_index.value().piece_type);
  bits::unset(piece_bitboard, move.start);
  bits::set(piece_bitboard, move.end);

  pos_data = {
      .player_to_move = get_opposite_color(pos_data.player_to_move),
      .castling_rights = pos_data.castling_rights,
      .en_passant_square = std::nullopt,
      .halfmove_clock = pos_data.halfmove_clock + 1,
      .fullmove_number =
          pos_data.fullmove_number + pos_data.player_to_move == BLACK ? 1 : 0,
      .captured_piece = std::nullopt,
      .next_move = pos_data.next_move,
  };
}

void BitboardsBoard::undo() {
  assert(history.size() >= 1);
  std::optional<BitboardIndex> piece_bitboard_index =
      find_bitboard_with_piece(pos_data.next_move.end);
  if (!piece_bitboard_index.has_value()) {
    throw std::invalid_argument(
        fmt::format("No piece on pos: {}", pos_data.next_move.end));
  }
  u_int64_t &piece_bitboard =
      bb_pieces.at(piece_bitboard_index.value().color)
          .at(piece_bitboard_index.value().piece_type);
  bits::unset(piece_bitboard, pos_data.next_move.end);
  bits::set(piece_bitboard, pos_data.next_move.start);

  history.pop_back();
  pos_data = history.back();
}

bool BitboardsBoard::is_draw() const { return false; }

bool BitboardsBoard::is_in_check(Color color) const { return false; }
