#pragma once

#include <array>
#include <optional>
#include <stack>
#include <vector>

#include "board.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "masks.hpp"
#include "piece.hpp"

// TODO: Maybe change squares to go from a1-h8 instead of a8-h1, might be more logical

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

class BitboardsBoard : public Board {
public:
  BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
                 std::array<Castling, 2> castling_rights,
                 std::optional<int> en_passant_square, int halfmove_clock,
                 int fullmove_number);

  bool operator==(const BitboardsBoard &other) const;

  Color get_player_to_move() const override;
  int get_halfmove_clock() const override;
  int get_fullmove_number() const override;
  std::optional<int> get_en_passant_square() const override;
  std::optional<Piece> get_captured_piece() const override;
  int get_material(Color color) const override;
  int get_psqt(Color color) const override;

  std::optional<PieceType> get_piece_type(int pos) const override;

  std::string to_string() const override;

  void make(const Move &move) override;
  void undo() override;

  bool is_draw() const override;
  bool is_in_check(Color color) const override;

  std::vector<Move>
  get_pseudo_legal_moves(MoveCategory move_category) const override;

private:
  std::array<std::array<u_int64_t, 6>, 2> piece_bbs;
  std::array<u_int64_t, 2> side_bbs;
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
  int get_castling_rook(const Move& move, Color color) const;

  std::vector<Move> gen_moves_piece(PieceType piece, int start,
                                    MoveCategory move_category) const;
  std::vector<Move> gen_all_moves_piece(PieceType piece,
                                        MoveCategory move_category) const;

  std::vector<Move> gen_knight_moves(int start) const;
  std::vector<Move> gen_pawn_moves(int start, MoveCategory move_category) const;

  std::vector<Move> gen_king_moves(int start, MoveCategory move_category) const;
  u_int64_t get_castling_check_not_allowed_bb(int start, bool kingside) const;
  u_int64_t get_castling_pieces_not_allowed_bb(int start, bool kingside) const;
  u_int64_t gen_castling_moves_bb(int start) const;

  u_int64_t gen_rook_attacks(int start, u_int64_t occupied) const;
  u_int64_t gen_bishop_attacks(int start, u_int64_t occupied) const;
  u_int64_t gen_queen_attacks(int start, u_int64_t occupied) const;

  u_int64_t gen_file_attacks(int start, u_int64_t occupied) const;
  u_int64_t gen_rank_attacks(int start, u_int64_t occupied) const;
  u_int64_t gen_diag_attacks(int start, u_int64_t occupied) const;
  u_int64_t gen_antidiag_attacks(int start, u_int64_t occupied) const;
  u_int64_t gen_sliding_attacks(int start, u_int64_t occupied,
                              u_int64_t mask) const;

  u_int64_t get_attacking_bb(Color color) const;
  bool is_attacking(int pos, Color color) const;

  bool is_lone_king(Color color) const;
  bool is_endgame() const;

  bool is_insufficient_material() const;
  bool is_draw_by_fifty_move_rule() const;
  bool is_threefold_repetition() const;
};
