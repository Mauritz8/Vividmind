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

  virtual Color player_to_move() const = 0;
  virtual int halfmove_clock() const = 0;
  virtual int fullmove_number() const = 0;
  virtual std::optional<int> en_passant_square() const = 0;
  virtual std::optional<Piece> captured_piece() const = 0;
  virtual int material(Color color) const = 0;
  virtual int psqt(Color color) const = 0;

  virtual std::optional<PieceType> piece_type(int pos) const = 0;

  virtual std::string to_string() const = 0;

  virtual void make(const Move &move) = 0;
  virtual void undo() = 0;

  virtual bool is_draw() const = 0;
  virtual bool is_in_check(Color color) const = 0;

  virtual std::vector<Move>
  get_pseudo_legal_moves(MoveCategory move_category) const = 0;
};
