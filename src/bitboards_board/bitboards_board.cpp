#include "bitboards_board.hpp"
#include "bits.hpp"
#include "defs.hpp"
#include "fmt/core.h"
#include "utils.hpp"
#include <optional>

BitboardsBoard::BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                               std::array<Castling, 2> castling_rights,
                               std::optional<int> en_passant_square,
                               int halfmove_clock, int fullmove_number) {

  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      bitboards_pieces.at(color).at(piece) = 0;
    }
  }
  for (const Piece &piece : pieces) {
    bits::set(bitboards_pieces.at(piece.color).at(piece.piece_type), piece.pos);
  }
}

bool BitboardsBoard::operator==(const BitboardsBoard &board) const {}

const std::optional<Piece> &BitboardsBoard::get_piece(int pos) const {}

Color BitboardsBoard::get_player_to_move() const {
  return game_state.player_to_move;
}

int BitboardsBoard::get_material(Color color) const {}

int BitboardsBoard::get_psqt(Color color) const {}

std::optional<PieceType> BitboardsBoard::get_piece_on_pos(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      u_int64_t bitboard_bit =
          bits::get(bitboards_pieces.at(color).at(piece), pos);
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

void BitboardsBoard::make(const Move &move) {}
void BitboardsBoard::undo() {}

bool BitboardsBoard::is_draw() const {}

bool BitboardsBoard::is_in_check(Color color) const {}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {}
