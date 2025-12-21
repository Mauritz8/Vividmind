#include "board.hpp"
#include "bits.hpp"
#include "defs.hpp"
#include "evaluation/evaluation.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include "move.hpp"
#include "utils.hpp"
#include <bit>
#include <cassert>
#include <optional>
#include <stdint.h>

Board::Board(std::vector<Piece> pieces, Color player_to_move,
             std::array<Castling, 2> castling_rights,
             std::optional<int> en_passant_square, int halfmove_clock,
             int fullmove_number)
    : masks(create_masks()) {

  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      piece_bbs.at(color).at(piece) = 0;
    }
  }
  for (const Piece &piece : pieces) {
    piece_bbs.at(piece.color).at(piece.piece_type) |=
        masks.squares.at(piece.pos);
  }

  std::array<int, 2> material;
  std::array<int, 2> psqt;
  for (int color = 0; color < 2; color++) {
    side_bbs.at(color) = 0;
    int material_side = 0;
    int psqt_side = 0;
    for (int piece = 0; piece < 6; piece++) {
      side_bbs.at(color) |= piece_bbs.at(color).at(piece);

      PieceType piece_type = (PieceType)piece;
      uint64_t piece_bb = piece_bbs.at(color).at(piece);
      material_side += std::popcount(piece_bb) * PIECE_VALUES.at(piece_type);

      while (piece_bb != 0) {
        int pos = bits::popLSB(piece_bb);
        psqt_side +=
            get_psqt_score(piece_type, pos, (Color)color, false, false);
      }
    }
    material.at(color) = material_side;
    psqt.at(color) = psqt_side;
  }

  std::stack<PosData> history;
  PosData pos_data = {
      .player_to_move = player_to_move,
      .castling_rights = castling_rights,
      .en_passant_square = en_passant_square,
      .halfmove_clock = halfmove_clock,
      .fullmove_number = fullmove_number,
      .captured_piece = std::nullopt,
      .material = material,
      .psqt = psqt,
  };
  history.push(pos_data);
  this->history = history;

  std::stack<Move> moves;
  this->move_history = moves;
}

Board Board::get_starting_position() {
  return fen::get_position(STARTING_POSITION_FEN);
}

Board Board::operator=(Board other) {
  piece_bbs = other.piece_bbs;
  side_bbs = other.side_bbs;
  history = other.history;
  move_history = other.move_history;
  return *this;
}

bool Board::operator==(const Board &other) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      if (this->piece_bbs.at(color).at(piece) !=
          other.piece_bbs.at(color).at(piece)) {
        return false;
      }
    }
  }
  return true;
}

bool Board::is_draw() const {
  return is_insufficient_material() || is_threefold_repetition() ||
         is_draw_by_fifty_move_rule();
}

std::optional<PieceType> Board::get_piece_type(int pos) const {
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      if (piece_at_pos(pos, (Color)color, (PieceType)piece)) {
        return (PieceType)piece;
      }
    }
  }
  return std::nullopt;
}

bool Board::piece_at_pos(int pos, Color color, PieceType piece_type) const {
  return ((piece_bbs.at(color).at(piece_type) >> pos) & (uint64_t)1) == 1;
}

Color Board::get_player_to_move() const { return history.top().player_to_move; }

int Board::get_halfmove_clock() const { return history.top().halfmove_clock; }
int Board::get_fullmove_number() const { return history.top().fullmove_number; }
std::optional<int> Board::get_en_passant_square() const {
  return history.top().en_passant_square;
}
std::optional<Piece> Board::get_captured_piece() const {
  return history.top().captured_piece;
}

int Board::get_material(Color color) const {
  return history.top().material.at(color);
}

int Board::get_psqt(Color color) const { return history.top().psqt.at(color); }

bool Board::is_lone_king(Color color) const {
  return std::popcount(side_bbs.at(color)) == 1;
}

bool Board::is_endgame() const {
  return get_material(WHITE) - KING_VALUE < 1500 &&
         get_material(BLACK) - KING_VALUE < 1500;
}

std::string Board::to_string() const {
  std::string board = "";
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      const int pos = row * 8 + col;
      std::optional<PieceType> piece = get_piece_type(pos);
      char ch =
          piece.has_value() ? get_char_representation(piece.value()) : '_';
      board += fmt::format("{} ", ch);
    }
    board += '\n';
  }

  return board;
}

std::array<Castling, 2> Board::updated_castling_rights(const Move &move) const {
  int kingside_rook;
  int queenside_rook;
  int king;
  int opponent_kingside_rook;
  int opponent_queenside_rook;
  if (get_player_to_move() == WHITE) {
    kingside_rook = 63;
    queenside_rook = 56;
    king = 60;
    opponent_kingside_rook = 7;
    opponent_queenside_rook = 0;
  } else {
    kingside_rook = 7;
    queenside_rook = 0;
    king = 4;
    opponent_kingside_rook = 63;
    opponent_queenside_rook = 56;
  }
  bool disable_kingside_player =
      move.start == kingside_rook || move.start == king;
  bool disable_queenside_player =
      move.start == queenside_rook || move.start == king;

  std::array<Castling, 2> castling_rights;
  castling_rights.at(get_player_to_move()) = {
      .kingside =
          disable_kingside_player
              ? false
              : history.top().castling_rights.at(get_player_to_move()).kingside,
      .queenside = disable_queenside_player
                       ? false
                       : history.top()
                             .castling_rights.at(get_player_to_move())
                             .queenside,
  };

  Color opponent = get_opposite_color(get_player_to_move());
  bool disable_kingside_opponent = move.end == opponent_kingside_rook;
  bool disable_queenside_opponent = move.end == opponent_queenside_rook;
  castling_rights.at(opponent) = {
      .kingside = disable_kingside_opponent
                      ? false
                      : history.top().castling_rights.at(opponent).kingside,
      .queenside = disable_queenside_opponent
                       ? false
                       : history.top().castling_rights.at(opponent).queenside,
  };

  return castling_rights;
}

int Board::get_castling_rook(const Move &move, Color color) const {
  int kingside = move.end > move.start;
  if (kingside) {
    return color == WHITE ? 63 : 7;
  } else {
    return color == WHITE ? 56 : 0;
  }
}

std::optional<PieceType> Board::piece_type(int pos, Color color) const {
  for (int piece = 0; piece < NR_PIECES; piece++) {
    if (piece_at_pos(pos, color, (PieceType)piece)) {
      return (PieceType)piece;
    }
  }
  return std::nullopt;
}

std::optional<Piece> Board::get_piece_to_be_captured(const Move &move) const {
  Color player = get_player_to_move();
  Color opponent = get_opposite_color(player);
  int pos = move.move_type == EN_PASSANT
                ? get_en_passant_square().value() + (player == WHITE ? 8 : -8)
                : move.end;
  std::optional<PieceType> piece_type_opt = piece_type(pos, opponent);
  return piece_type_opt.has_value()
             ? std::optional<Piece>(
                   Piece(piece_type_opt.value(), opponent, pos))
             : std::nullopt;
}

std::array<int, 2>
Board::updated_material(const Move &move,
                        std::optional<Piece> captured_piece) const {
  const Color player_to_move = get_player_to_move();
  const Color opponent = get_opposite_color(player_to_move);

  std::array<int, 2> material;
  material.at(player_to_move) =
      get_material(player_to_move) +
      (move.move_type == PROMOTION
           ? PIECE_VALUES.at(move.promotion_piece.value()) -
                 PIECE_VALUES.at(PAWN)
           : 0);
  material.at(opponent) =
      get_material(opponent) -
      (captured_piece.has_value()
           ? PIECE_VALUES.at(captured_piece.value().piece_type)
           : 0);
  return material;
}

std::array<int, 2>
Board::updated_psqt(const Move &move,
                    std::optional<Piece> captured_piece) const {
  const Color player_to_move = get_player_to_move();
  const Color opponent = get_opposite_color(player_to_move);

  const std::optional<PieceType> piece_type_optional =
      piece_type(move.start, player_to_move);
  assert(piece_type_optional.has_value());
  const PieceType piece_type = piece_type_optional.value();
  const PieceType new_piece_type =
      move.move_type == PROMOTION ? move.promotion_piece.value() : piece_type;

  bool endgame = piece_type == PieceType::KING ? is_endgame() : false;
  bool lone_king =
      piece_type == PieceType::KING ? is_lone_king(player_to_move) : false;
  std::array<int, 2> psqt;
  psqt.at(player_to_move) = get_psqt(player_to_move) -
                            get_psqt_score(piece_type, move.start,
                                           player_to_move, lone_king, endgame) +
                            get_psqt_score(new_piece_type, move.end,
                                           player_to_move, lone_king, endgame);
  if (move.move_type == CASTLING) {
    const int kingside = move.end > move.start;
    const int rook_start = get_castling_rook(move, player_to_move);
    const int rook_end = rook_start + (kingside ? -2 : 3);
    psqt.at(player_to_move) +=
        get_psqt_score(ROOK, rook_end, player_to_move, false, false) -
        get_psqt_score(ROOK, rook_start, player_to_move, false, false);
  }

  psqt.at(opponent) =
      get_psqt(opponent) -
      (captured_piece.has_value()
           ? get_psqt_score(captured_piece.value().piece_type,
                            captured_piece.value().pos,
                            captured_piece.value().color, false, false)
           : 0);
  return psqt;
}

void Board::make(const Move &move) {

  const Color player_to_move = get_player_to_move();
  const std::optional<PieceType> piece_type_opt =
      piece_type(move.start, player_to_move);
  assert(piece_type_opt.has_value());
  const PieceType piece_type = piece_type_opt.value();

  const std::optional<Piece> captured_piece_opt =
      get_piece_to_be_captured(move);

  const PosData new_pos_data = {
      .player_to_move = get_opposite_color(player_to_move),
      .castling_rights = updated_castling_rights(move),
      .en_passant_square = move.move_type == PAWN_TWO_SQUARES_FORWARD
                               ? std::optional<int>((move.start + move.end) / 2)
                               : std::nullopt,
      .halfmove_clock = piece_type == PAWN || captured_piece_opt.has_value()
                            ? 0
                            : history.top().halfmove_clock + 1,
      .fullmove_number =
          history.top().fullmove_number + (player_to_move == BLACK ? 1 : 0),
      .captured_piece = captured_piece_opt,
      .material = updated_material(move, captured_piece_opt),
      .psqt = updated_psqt(move, captured_piece_opt),
  };

  history.push(new_pos_data);
  move_history.push(move);

  const PieceType new_piece_type =
      move.move_type == PROMOTION && move.promotion_piece.has_value()
          ? move.promotion_piece.value()
          : piece_type;
  remove_piece(move.start, piece_type, player_to_move);
  add_piece(move.end, new_piece_type, player_to_move);

  if (move.move_type == CASTLING) {
    const int rook = get_castling_rook(move, player_to_move);
    const int kingside = move.end > move.start;
    const int rook_new = kingside ? rook - 2 : rook + 3;
    remove_piece(rook, ROOK, player_to_move);
    add_piece(rook_new, ROOK, player_to_move);
  }

  if (captured_piece_opt.has_value()) {
    const Piece p = captured_piece_opt.value();
    remove_piece(p.pos, p.piece_type, p.color);
  }
}

void Board::undo() {
  assert(move_history.size() >= 1);

  const Move &move = move_history.top();

  const Color move_played_by = get_opposite_color(get_player_to_move());
  const std::optional<PieceType> piece_type_opt =
      piece_type(move.end, move_played_by);
  assert(piece_type_opt.has_value());
  const PieceType piece_type = piece_type_opt.value();

  uint64_t &piece_bb = piece_bbs.at(move_played_by).at(piece_type);
  uint64_t &side_bb = side_bbs.at(move_played_by);

  const PieceType old_piece_type =
      move.move_type == PROMOTION ? PAWN : piece_type;
  remove_piece(move.end, piece_type, move_played_by);
  add_piece(move.start, old_piece_type, move_played_by);

  if (move.move_type == CASTLING) {
    int kingside = move.end > move.start;
    int rook = get_castling_rook(move, move_played_by);
    int rook_new = kingside ? rook - 2 : rook + 3;
    uint64_t &rook_bb = piece_bbs.at(move_played_by).at(ROOK);
    remove_piece(rook_new, ROOK, move_played_by);
    add_piece(rook, ROOK, move_played_by);
  }

  const std::optional<Piece> captured_piece_opt = history.top().captured_piece;
  if (captured_piece_opt.has_value()) {
    const Piece p = captured_piece_opt.value();
    remove_piece(p.pos, p.piece_type, p.color);
  }

  history.pop();
  move_history.pop();
}

void Board::add_piece(int pos, PieceType piece_type, Color color) {
  piece_bbs.at(color).at(piece_type) |= masks.squares.at(pos);
  side_bbs.at(color) |= masks.squares.at(pos);
}

void Board::remove_piece(int pos, PieceType piece_type, Color color) {
  piece_bbs.at(color).at(piece_type) ^= masks.squares.at(pos);
  side_bbs.at(color) ^= masks.squares.at(pos);
}

bool Board::is_insufficient_material() const {
  uint64_t all_pieces_bb = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  if (std::popcount(all_pieces_bb) > 3) {
    return false;
  }

  std::array<PieceType, 3> pieces = {PAWN, ROOK, QUEEN};
  for (PieceType p : pieces) {
    for (int color = 0; color < 2; color++) {
      if (piece_bbs.at(color).at(p) != 0) {
        return false;
      }
    }
  }
  return true;
}

bool Board::is_draw_by_fifty_move_rule() const {
  return history.top().halfmove_clock > 100;
}

bool Board::is_threefold_repetition() const {
  if (history.top().halfmove_clock < 5) {
    return false;
  }

  Board b = *this;
  while (!(b.move_history.empty() || history.top().halfmove_clock == 0)) {
    b.undo();
    if (*this == b) {
      return true;
    }
  }
  return false;
}

int Board::get_doubled_pawns(Color color) const {
  uint64_t pawn_bb = piece_bbs.at(color).at(PieceType::PAWN);
  int doubled_pawns = 0;
  for (int i = 0; i < 8; i++) {
    uint64_t pawns_file = pawn_bb & masks.files.at(i);
    if (std::popcount(pawns_file) > 1) {
      doubled_pawns++;
    }
  }
  return doubled_pawns;
}
