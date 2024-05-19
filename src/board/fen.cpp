#include "board.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "board/defs.hpp"
#include "evaluation/evaluation.hpp"
#include "fen.hpp"
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
    throw std::invalid_argument("Invalid FEN: " + player_to_move +
                                " is not a valid color\n");
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

std::array<Square, 64> get_squares(std::string_view pieces) {
  std::array<Square, 64> squares;
  int pos = 0;
  for (const char ch : pieces) {
    if (ch == '/')
      continue;

    if (isdigit(ch)) {
      const int n = (int)ch - '0';
      for (int _ = 0; _ < n; _++) {
        squares.at(pos) = Square(pos);
        pos++;
      }
    } else {
      Color color = islower(ch) ? BLACK : WHITE;
      Piece piece = Piece(get_piece_type(ch), color, pos);
      squares.at(pos) = Square(pos, piece);
      pos++;
    }
  }
  return squares;
}

Board get_position(std::string_view fen) {
  std::vector<std::string> fen_parts = str_split(fen, ' ');
  if (fen_parts.size() != 6) {
    throw std::invalid_argument("Invalid FEN: Does not contain six parts\n");
  }

  std::array<Square, 64> squares = get_squares(fen_parts.at(0));
  std::vector<Piece> pieces;
  for (Square s : squares) {
    if (s.piece) {
      pieces.push_back(*s.piece);
    }
  }

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
  int white_psqt =
      std::accumulate(white_pieces.begin(), white_pieces.end(), 0,
                      [](int v, Piece p) { return v + get_psqt_score(p); });
  int black_psqt =
      std::accumulate(black_pieces.begin(), black_pieces.end(), 0,
                      [](int m, Piece p) { return m + get_psqt_score(p); });

  GameState game_state = {
      .player_to_move = calc_player_to_move(fen_parts.at(1)),
      .castling_rights = calc_castling_rights(fen_parts.at(2)),
      .en_passant_square = calc_en_passant_square(fen_parts.at(3)),
      .halfmove_clock = calc_halfmove_clock(fen_parts.at(4)),
      .fullmove_number = calc_fullmove_number(fen_parts.at(5)),
      .material = {white_material, black_material},
      .psqt = {white_psqt, black_psqt},
      .captured_piece = std::nullopt,
  };

  return Board(squares, {white_pieces, black_pieces}, game_state, {});
}
} // namespace fen
