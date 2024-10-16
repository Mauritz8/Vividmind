#include "board.hpp"
#include "../fen.hpp"
#include "board/defs.hpp"
#include "evaluation/evaluation.hpp"
#include "fmt/core.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <vector>

Board::Board(std::vector<Piece> pieces, Color player_to_move,
             std::array<Castling, 2> castling_rights,
             std::optional<int> en_passant_square, int halfmove_clock,
             int fullmove_number)
    : history({}) {
  std::array<Square, 64> squares;
  for (int i = 0; i < 64; i++) {
    squares.at(i) = Square(i);
  }
  for (const Piece &piece : pieces) {
    squares.at(piece.pos).piece = piece;
  }
  this->squares = squares;

  std::vector<Piece> white_pieces;
  std::copy_if(pieces.begin(), pieces.end(), std::back_inserter(white_pieces),
               [](Piece p) { return p.color == WHITE; });

  std::vector<Piece> black_pieces;
  std::copy_if(pieces.begin(), pieces.end(), std::back_inserter(black_pieces),
               [](Piece p) { return p.color == BLACK; });

  int white_material =
      std::accumulate(white_pieces.begin(), white_pieces.end(), 0,
                      [](int v, Piece p) { return v + p.get_value(); });
  int black_material =
      std::accumulate(black_pieces.begin(), black_pieces.end(), 0,
                      [](int v, Piece p) { return v + p.get_value(); });

  auto is_lone_king = [&](Color color) {
    const std::vector pieces = color == WHITE ? white_pieces : black_pieces;
    return pieces.size() == 1;
  };
  bool is_endgame =
      white_material - KING_VALUE < 1500 && black_material - KING_VALUE < 1500;
  int white_psqt = std::accumulate(
      white_pieces.begin(), white_pieces.end(), 0, [&](int v, Piece p) {
        return v + ::get_psqt_score(p, is_lone_king(p.color), is_endgame);
      });
  int black_psqt = std::accumulate(
      black_pieces.begin(), black_pieces.end(), 0, [&](int m, Piece p) {
        return m + ::get_psqt_score(p, is_lone_king(p.color), is_endgame);
      });

  this->game_state = {
      .player_to_move = player_to_move,
      .castling_rights = castling_rights,
      .en_passant_square = en_passant_square,
      .halfmove_clock = halfmove_clock,
      .fullmove_number = fullmove_number,
      .pieces = {white_pieces, black_pieces},
      .material = {white_material, black_material},
      .psqt = {white_psqt, black_psqt},
      .captured_piece = std::nullopt,
  };
}

Board Board::get_starting_position() {
  return fen::get_position(STARTING_POSITION_FEN);
}

bool Board::operator==(const Board &other) const {
  for (int pos = 0; pos < 64; pos++) {
    if (squares.at(pos).piece != other.squares.at(pos).piece) {
      return false;
    }
  }

  if (game_state.en_passant_square != other.game_state.en_passant_square) {
    return false;
  }

  std::array<Color, 2> colors = {WHITE, BLACK};
  for (Color color : colors) {
    if (game_state.castling_rights.at(color).kingside !=
        other.game_state.castling_rights.at(color).kingside) {
      return false;
    }
    if (game_state.castling_rights.at(color).queenside !=
        other.game_state.castling_rights.at(color).queenside) {
      return false;
    }
  }

  return true;
}

const std::optional<Piece> &Board::get_piece(int pos) const {
  return squares.at(pos).piece;
}

const Color Board::get_player_to_move() const {
  return game_state.player_to_move;
}

const int Board::get_material(Color color) const {
  return game_state.material.at(color);
}

const int Board::get_psqt(Color color) const {
  return game_state.psqt.at(color);
}

std::string Board::to_string() const {
  auto to_str = [](std::string str, Square s) {
    bool last_col = (s.pos + 1) % 8 == 0;
    return fmt::format("{} {}{}", str,
                       s.piece ? s.piece->get_char_representation() : '_',
                       last_col ? "\n" : "");
  };
  std::string board =
      std::accumulate(squares.begin(), squares.end(), std::string(""), to_str);
  return board;
}

int Board::get_king_square(Color color) const {
  auto is_king = [](Piece p) { return p.piece_type == KING; };
  const std::vector pieces = game_state.pieces.at(color);
  auto king = std::find_if(pieces.begin(), pieces.end(), is_king);
  if (king != pieces.end()) {
    return king->pos;
  }
  throw std::invalid_argument(fmt::format("No {} king on the board",
                                          color == WHITE ? "white" : "black"));
}
