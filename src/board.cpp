#include "board.hpp"

#include <array>
#include <iostream>
#include <vector>

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

void Board::show() const {
  for (int i = 0; i < 64; i++) {
    const std::optional<Piece> &piece = this->squares.at(i).piece;
    if (piece) {
      std::cout << " " << piece->get_char_representation();
    } else {
      std::cout << " _";
    }

    if ((i + 1) % 8 == 0) {
      std::cout << "\n";
    }
  }
  std::cout << "\n";
}

void Board::switch_player_to_move() {
  if (this->game_state.player_to_move == WHITE) {
    this->game_state.player_to_move = BLACK;
  } else {
    this->game_state.player_to_move = WHITE;
  }
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
