#include "board.hpp"
#include "board/defs.hpp"
#include "board/fen.hpp"

#include <array>
#include <iostream>
#include <numeric>
#include <vector>

Board::Board(std::array<Square, 64> squares,
             std::array<std::vector<Piece>, 2> pieces, GameState game_state,
             std::vector<GameState> history)
    : squares(squares), pieces(pieces), game_state(game_state),
      history(history) {}

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

std::string Board::to_string() const {
  auto to_str = [](std::string str, Square square) {
    bool last_col = (square.pos + 1) % 8 == 0;
    return str + " " +
           (square.piece ? square.piece->get_char_representation() : '_') +
           (last_col ? "\n" : "");
  };
  std::string board =
      std::accumulate(squares.begin(), squares.end(), std::string(""), to_str);
  return board + "\n";
}

int Board::get_king_square(Color color) const {
  for (Piece piece : pieces.at(color)) {
    if (piece.piece_type == KING) {
      return piece.pos;
    }
  }
  std::string color_str = color == WHITE ? "white" : "black";
  throw std::invalid_argument("No " + color_str + " king on the board");
}
