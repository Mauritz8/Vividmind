#pragma once

#include <array>
#include <vector>

#include "defs.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "bitboards_board.hpp"

struct GameState {
  Color player_to_move;
  std::array<Castling, 2> castling_rights;
  std::optional<int> en_passant_square;
  int halfmove_clock;
  int fullmove_number;
};

class BitboardsBoard {
public:
  BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                 std::array<Castling, 2> castling_rights,
                 std::optional<int> en_passant_square, int halfmove_clock,
                 int fullmove_number);

  static BitboardsBoard get_starting_position();

  bool operator==(const BitboardsBoard &board) const;

  const std::optional<Piece> &get_piece(int pos) const;
  Color get_player_to_move() const;
  int get_material(Color color) const;
  int get_psqt(Color color) const;

  std::string to_string() const;

  void make(const Move &move);
  void undo();

  bool is_draw() const;
  bool is_in_check(Color color) const;

  std::vector<Move> get_pseudo_legal_moves(MoveCategory move_category) const;

private:
  std::array<std::array<u_int64_t, 6>, 2> bitboards_pieces;
  GameState game_state;
};
