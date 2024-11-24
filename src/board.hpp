#pragma once

#include "move.hpp"
#include "piece.hpp"
#include <memory>
#include <optional>
#include <vector>

class Board {
public:
  virtual ~Board() {}

  static std::unique_ptr<Board> get_starting_position();

  virtual Color get_player_to_move() const = 0;
  virtual int get_halfmove_clock() const = 0;
  virtual int get_fullmove_number() const = 0;
  virtual std::optional<int> get_en_passant_square() const = 0;
  virtual std::optional<Piece> get_captured_piece() const = 0;
  virtual int get_material(Color color) const = 0;
  virtual int get_psqt(Color color) const = 0;

  virtual std::optional<PieceType> get_piece_type(int pos) const = 0;

  virtual std::string to_string() const = 0;

  virtual void make(const Move &move) = 0;
  virtual void undo() = 0;

  virtual bool is_in_check(Color color) const = 0;

  bool is_draw() const;
  virtual bool is_insufficient_material() const = 0;
  virtual bool is_draw_by_fifty_move_rule() const = 0;
  virtual bool is_threefold_repetition() const = 0;

  virtual std::vector<Move>
  get_pseudo_legal_moves(MoveCategory move_category) const = 0;
};
