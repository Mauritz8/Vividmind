#pragma once

#include <array>
#include <vector>

#include "bitboards_board.hpp"
#include "board.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "piece.hpp"

struct GameState {
  Color player_to_move;
  std::array<Castling, 2> castling_rights;
  std::optional<int> en_passant_square;
  int halfmove_clock;
  int fullmove_number;
};

class BitboardsBoard : public Board {
public:
  BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                 std::array<Castling, 2> castling_rights,
                 std::optional<int> en_passant_square, int halfmove_clock,
                 int fullmove_number);

  bool operator==(const BitboardsBoard &board) const;

  const std::optional<Piece> &get_piece(int pos) const override;
  Color get_player_to_move() const override;
  int get_material(Color color) const override;
  int get_psqt(Color color) const override;

  std::string to_string() const override;

  void make(const Move &move) override;
  void undo() override;

  bool is_draw() const override;
  bool is_in_check(Color color) const override;

  std::vector<Move>
  get_pseudo_legal_moves(MoveCategory move_category) const override;

private:
  std::array<std::array<u_int64_t, 6>, 2> bitboards_pieces;
  GameState game_state;

  std::optional<PieceType> get_piece_on_pos(int pos) const;
};
