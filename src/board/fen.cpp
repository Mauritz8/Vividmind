#include "board.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

#include "board/defs.hpp"
#include "utils.hpp"

Board Board::get_starting_position() {
  return get_position_from_fen(STARTING_POSITION_FEN);
}

Board Board::get_position_from_fen(const std::string &fen) {
  std::istringstream fen_stream(fen);
  std::array<std::string, 6> fen_parts;

  int i = 0;
  std::string fen_part;
  while (std::getline(fen_stream, fen_part, ' ')) {
    fen_parts.at(i++) = fen_part;
  }

  if (i != 6) {
    throw std::invalid_argument("Invalid FEN: Does not contain six parts\n");
  }

  Board board;
  board.game_state.material.at(WHITE) = 0;
  board.game_state.material.at(BLACK) = 0;
  board.game_state.psqt.at(WHITE) = 0;
  board.game_state.psqt.at(BLACK) = 0;
  board.place_pieces(fen_parts.at(0));
  board.game_state.player_to_move = board.calc_player_to_move(fen_parts.at(1));
  board.game_state.castling_rights =
      board.calc_castling_rights(fen_parts.at(2));
  board.game_state.en_passant_square =
      board.calc_en_passant_square(fen_parts.at(3));
  board.game_state.halfmove_clock = board.calc_halfmove_clock(fen_parts.at(4));
  board.game_state.fullmove_number =
      board.calc_fullmove_number(fen_parts.at(5));

  return board;
}

void Board::place_pieces(const std::string &pieces) {
  int pos = 0;
  for (const char ch : pieces) {
    if (ch == '/')
      continue;

    if (isdigit(ch)) {
      const int n = (int)ch - '0';
      for (int i = 0; i < n; i++) {
        squares.at(pos).pos = pos;
        pos++;
      }
    } else {
      Color color = islower(ch) ? BLACK : WHITE;
      Piece piece = Piece(get_piece_type(ch), color, pos);
      Square &square = squares.at(pos);
      squares.at(pos).pos = pos;
      squares.at(pos).piece = piece;
      this->pieces.at(color).push_back(piece);
      game_state.material.at(color) += piece.get_value();
      game_state.psqt.at(color) += get_psqt_score(piece);
      pos++;
    }
  }
}

Color Board::calc_player_to_move(const std::string &player_to_move) {
  if (player_to_move == "w") {
    return WHITE;
  } else if (player_to_move == "b") {
    return BLACK;
  } else {
    throw std::invalid_argument("Invalid FEN: " + player_to_move +
                                " is not a valid color\n");
  }
}

std::array<Castling, 2>
Board::calc_castling_rights(const std::string &castling_rights_str) {
  std::array<Castling, 2> castling_rights;
  castling_rights.at(WHITE).kingside = false;
  castling_rights.at(WHITE).queenside = false;
  castling_rights.at(BLACK).kingside = false;
  castling_rights.at(BLACK).queenside = false;

  for (char ch : castling_rights_str) {
    switch (ch) {
    case 'K':
      castling_rights.at(WHITE).kingside = true;
      break;
    case 'Q':
      castling_rights.at(WHITE).queenside = true;
      break;
    case 'k':
      castling_rights.at(BLACK).kingside = true;
      break;
    case 'q':
      castling_rights.at(BLACK).queenside = true;
      break;
    }
  }
  return castling_rights;
}

std::optional<int>
Board::calc_en_passant_square(const std::string &en_passant_square) {
  if (en_passant_square.size() != 2) {
    return {};
  }
  const int x = en_passant_square.at(0) - 'a';
  const int y = 8 - (en_passant_square.at(1) - '0');
  return x + y * 8;
}

int Board::calc_halfmove_clock(const std::string &halfmove_clock) {
  try {
    return std::stoi(halfmove_clock);
  } catch (const std::invalid_argument &e) {
    throw std::invalid_argument(
        "Invalid FEN: not a valid halfmove clock value\n");
  }
}

int Board::calc_fullmove_number(const std::string &fullmove_number) {
  try {
    return std::stoi(fullmove_number);
  } catch (const std::invalid_argument &e) {
    throw std::invalid_argument("Invalid FEN: not a valid fullmove number\n");
  }
}
