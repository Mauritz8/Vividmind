#pragma once

#include <array>
#include <optional>
#include <vector>

#include "bitboards_board.hpp"
#include "board.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "move_gen_lookup_tables.hpp"
#include "piece.hpp"

struct PosData {
  Color player_to_move;
  std::array<Castling, 2> castling_rights;
  std::optional<int> en_passant_square;
  int halfmove_clock;
  int fullmove_number;
  std::optional<Piece> captured_piece;
  Move next_move;
};

struct BitboardIndex {
  Color color;
  PieceType piece_type;
};

class BitboardsBoard : public Board {
public:
  BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                 std::array<Castling, 2> castling_rights,
                 std::optional<int> en_passant_square, int halfmove_clock,
                 int fullmove_number);

  bool operator==(const BitboardsBoard &other) const;

  std::optional<PieceType> get_piece_type(int pos) const override;
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
  std::array<std::array<u_int64_t, 6>, 2> bb_pieces;
  PosData pos_data;
  std::vector<PosData> history;
  MoveGenLookupTables move_gen_lookup_tables;

  std::optional<PieceType> get_piece_on_pos(int pos) const;
  std::optional<BitboardIndex> find_bitboard_with_piece(int pos) const;
  std::optional<Piece> remove_piece(int pos);

  std::vector<Move>
  gen_moves(u_int64_t bb_start_squares,
            std::array<u_int64_t, 64> bb_end_squares_lookup_table) const;
};
