#include "board.hpp"
#include "board/defs.hpp"
#include "board/fen.hpp"
#include "fmt/core.h"

#include <algorithm>
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
  auto king =
      std::find_if(pieces.at(color).begin(), pieces.at(color).end(), is_king);
  if (king != pieces.at(color).end()) {
    return king->pos;
  }
  throw std::invalid_argument(fmt::format("No {} king on the board",
                                          color == WHITE ? "white" : "black"));
}
