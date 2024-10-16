#include "board/board.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "board/board_defs.hpp"
#include "evaluation/evaluation.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include "utils.hpp"

namespace fen {

int get_psqt_score(Piece piece) {
  const int square =
      piece.color == WHITE ? piece.pos : get_mirrored_pos(piece.pos);
  switch (piece.piece_type) {
  case KING: {
    return KING_PSQT.at(square);
  }
  case QUEEN:
    return QUEEN_PSQT.at(square);
  case ROOK:
    return ROOK_PSQT.at(square);
  case BISHOP:
    return BISHOP_PSQT.at(square);
  case KNIGHT:
    return KNIGHT_PSQT.at(square);
  case PAWN:
    return PAWN_PSQT.at(square);
  }
}

Color calc_player_to_move(const std::string &player_to_move) {
  if (player_to_move == "w") {
    return WHITE;
  } else if (player_to_move == "b") {
    return BLACK;
  } else {
    throw std::invalid_argument(
        fmt::format("Invalid FEN: {} is not a valid color\n", player_to_move));
  }
}

std::array<Castling, 2>
calc_castling_rights(const std::string &castling_rights_str) {
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
calc_en_passant_square(const std::string &en_passant_square) {
  if (en_passant_square.size() != 2) {
    return {};
  }
  const int x = en_passant_square.at(0) - 'a';
  const int y = 8 - (en_passant_square.at(1) - '0');
  return x + y * 8;
}

int calc_halfmove_clock(const std::string &halfmove_clock) {
  try {
    return std::stoi(halfmove_clock);
  } catch (const std::invalid_argument &e) {
    throw std::invalid_argument(
        "Invalid FEN: not a valid halfmove clock value\n");
  }
}

int calc_fullmove_number(const std::string &fullmove_number) {
  try {
    return std::stoi(fullmove_number);
  } catch (const std::invalid_argument &e) {
    throw std::invalid_argument("Invalid FEN: not a valid fullmove number\n");
  }
}

std::vector<Piece> get_pieces(std::string_view pieces_str) {
  std::vector<Piece> pieces;
  int pos = 0;
  for (const char ch : pieces_str) {
    if (ch == '/') {
      continue;
    }

    if (isdigit(ch)) {
      const int n = (int)ch - '0';
      pos += n;
    } else {
      Color color = islower(ch) ? BLACK : WHITE;
      Piece piece = Piece(get_piece_type(ch), color, pos);
      pieces.push_back(piece);
      pos++;
    }
  }
  return pieces;
}

Board get_position(std::string_view fen) {
  std::vector<std::string> fen_parts = str_split(fen, ' ');
  if (fen_parts.size() != 6) {
    throw std::invalid_argument("Invalid FEN: Does not contain six parts\n");
  }

  std::vector<Piece> pieces = get_pieces(fen_parts.at(0));
  const Color player_to_move = calc_player_to_move(fen_parts.at(1));
  const std::array<Castling, 2> castling_rights =
      calc_castling_rights(fen_parts.at(2));
  const std::optional<int> en_passant_square =
      calc_en_passant_square(fen_parts.at(3));
  const int halfmove_clock = calc_halfmove_clock(fen_parts.at(4));
  const int fullmove_number = calc_fullmove_number(fen_parts.at(5));

  return Board(pieces, player_to_move, castling_rights, en_passant_square,
               halfmove_clock, fullmove_number);
}
} // namespace fen
