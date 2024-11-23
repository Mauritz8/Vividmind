#include "mailbox_board.hpp"
#include "evaluation/evaluation.hpp"
#include "fmt/core.h"
#include "mailbox_board/mailbox_board_defs.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <vector>

MailboxBoard::MailboxBoard(std::vector<Piece> pieces, Color player_to_move,
                           std::array<Castling, 2> castling_rights,
                           std::optional<int> en_passant_square,
                           int halfmove_clock, int fullmove_number)
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
        return v + ::get_psqt_score(p.piece_type, p.pos, p.color,
                                    is_lone_king(p.color), is_endgame);
      });
  int black_psqt = std::accumulate(
      black_pieces.begin(), black_pieces.end(), 0, [&](int m, Piece p) {
        return m + ::get_psqt_score(p.piece_type, p.pos, p.color,
                                    is_lone_king(p.color), is_endgame);
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

bool MailboxBoard::operator==(const MailboxBoard &other) const {
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

std::optional<PieceType> MailboxBoard::piece_type(int pos) const {
  std::optional<Piece> piece = squares.at(pos).piece;
  return piece.has_value() ? std::optional<PieceType>(piece.value().piece_type)
                           : std::nullopt;
}

Color MailboxBoard::player_to_move() const {
  return game_state.player_to_move;
}

int MailboxBoard::halfmove_clock() const {
  return game_state.halfmove_clock;
}

int MailboxBoard::fullmove_number() const {
  return game_state.fullmove_number;
}

std::optional<int> MailboxBoard::en_passant_square() const {
  return game_state.en_passant_square;
}

std::optional<Piece> MailboxBoard::captured_piece() const {
  return game_state.captured_piece;
}

int MailboxBoard::material(Color color) const {
  return game_state.material.at(color);
}

int MailboxBoard::psqt(Color color) const {
  return game_state.psqt.at(color);
}

std::string MailboxBoard::to_string() const {
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

int MailboxBoard::get_king_square(Color color) const {
  auto is_king = [](Piece p) { return p.piece_type == KING; };
  const std::vector pieces = game_state.pieces.at(color);
  auto king = std::find_if(pieces.begin(), pieces.end(), is_king);
  if (king != pieces.end()) {
    return king->pos;
  }
  throw std::invalid_argument(fmt::format("No {} king on the board",
                                          color == WHITE ? "white" : "black"));
}

int MailboxBoard::get_psqt_score(const Piece &piece) const {
  return ::get_psqt_score(piece.piece_type, piece.pos, piece.color,
                          is_lone_king(piece.color), is_endgame());
}

bool MailboxBoard::is_lone_king(Color color) const {
  return game_state.pieces.at(color).size() == 1;
}

bool MailboxBoard::is_endgame() const {
  return game_state.material.at(WHITE) - KING_VALUE < 1500 &&
         game_state.material.at(BLACK) - KING_VALUE < 1500;
}
