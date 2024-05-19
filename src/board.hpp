#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "board/defs.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"

class Board {
public:
  std::array<Square, 64> squares;
  std::array<std::vector<Piece>, 2> pieces;
  GameState game_state;
  std::vector<GameState> history;

  Board(std::array<Square, 64> squares,
        std::array<std::vector<Piece>, 2> pieces, GameState game_state,
        std::vector<GameState> history);

  static Board get_starting_position();

  bool operator==(const Board &board) const;

  std::string to_string() const;
  int get_king_square(Color color) const;

  void make(const Move &move);
  void undo();

  bool is_insufficient_material() const;
  bool is_threefold_repetition() const;
  bool is_draw_by_fifty_move_rule() const;

  int evaluate() const;

private:
  Piece &get_piece(Piece piece);
  void remove_piece(Piece piece);
  void move_piece(Square &from, Square &to);
  void update_castling_rights(const Move &move);
  Move get_castling_rook_move(const Move &move) const;

  int get_psqt_score(const Piece &piece) const;
  bool is_lone_king(Color color) const;
  bool is_endgame() const;
};
