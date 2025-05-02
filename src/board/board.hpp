#pragma once

#include <array>
#include <optional>
#include <stack>
#include <vector>

#include "defs.hpp"
#include "masks.hpp"
#include "move.hpp"
#include "piece.hpp"

// TODO: Maybe change squares to go from a1-h8 instead of a8-h1, might be more
// logical

struct PosData {
  Color player_to_move;
  std::array<Castling, 2> castling_rights;
  std::optional<int> en_passant_square;
  int halfmove_clock;
  int fullmove_number;
  std::optional<Piece> captured_piece;
  std::array<int, 2> material;
  std::array<int, 2> psqt;
};

const int NR_PIECES = 6;

class Board {
public:
  Board(std::vector<Piece> pieces, Color player_to_move,
        std::array<Castling, 2> castling_rights,
        std::optional<int> en_passant_square, int halfmove_clock,
        int fullmove_number);

  static Board get_starting_position();

  Board operator=(Board other);
  bool operator==(const Board &other) const;

  Color get_player_to_move() const;
  int get_halfmove_clock() const;
  int get_fullmove_number() const;
  std::optional<int> get_en_passant_square() const;
  std::optional<Piece> get_captured_piece() const;
  int get_material(Color color) const;
  int get_psqt(Color color) const;
  int get_doubled_pawns(Color color) const;

  std::optional<PieceType> get_piece_type(int pos) const;

  std::string to_string() const;

  void make(const Move &move);
  void undo();

  bool is_in_check(Color color) const;

  std::vector<Move> get_pseudo_legal_moves(MoveCategory move_category) const;

  bool is_draw() const;
  bool is_insufficient_material() const;
  bool is_draw_by_fifty_move_rule() const;
  bool is_threefold_repetition() const;

private:
  std::array<std::array<uint64_t, 6>, 2> piece_bbs;
  std::array<uint64_t, 2> side_bbs;
  std::stack<PosData> history;
  std::stack<Move> move_history;
  const Masks masks;

  std::optional<Piece> get_piece_to_be_captured(const Move &move) const;
  std::array<int, 2>
  updated_material(const Move &move, std::optional<Piece> captured_piece) const;
  std::array<int, 2> updated_psqt(const Move &move,
                                  std::optional<Piece> captured_piece) const;

  std::optional<PieceType> piece_type(int pos, Color color) const;
  std::array<Castling, 2> updated_castling_rights(const Move &move) const;
  int get_castling_rook(const Move &move, Color color) const;

  void gen_moves_piece(PieceType piece, int start, MoveCategory move_category,
                       std::vector<Move> &moves) const;
  void gen_all_moves_piece(PieceType piece, MoveCategory move_category,
                           std::vector<Move> &moves) const;

  std::vector<Move> gen_knight_moves(int start) const;
  void gen_pawn_moves(int start, MoveCategory move_category,
                      std::vector<Move> &moves) const;

  void gen_king_moves(int start, MoveCategory move_category,
                      std::vector<Move> &moves) const;
  uint64_t get_castling_check_not_allowed_bb(int start, bool kingside) const;
  uint64_t get_castling_pieces_not_allowed_bb(int start, bool kingside) const;
  uint64_t gen_castling_moves_bb(int start) const;

  uint64_t gen_rook_attacks(int start, uint64_t occupied) const;
  uint64_t gen_bishop_attacks(int start, uint64_t occupied) const;
  uint64_t gen_queen_attacks(int start, uint64_t occupied) const;

  uint64_t gen_file_attacks(int start, uint64_t occupied) const;
  uint64_t gen_rank_attacks(int start, uint64_t occupied) const;
  uint64_t gen_diag_attacks(int start, uint64_t occupied) const;
  uint64_t gen_antidiag_attacks(int start, uint64_t occupied) const;
  uint64_t gen_sliding_attacks(int start, uint64_t occupied,
                               uint64_t mask) const;

  uint64_t get_attacking_bb(Color color) const;
  bool is_attacking(int pos, Color color) const;

  bool is_lone_king(Color color) const;
  bool is_endgame() const;
};
