#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "board.hpp"
#include "defs.hpp"
#include "mailbox_board_defs.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"

class MailboxBoard : public Board {
public:
  MailboxBoard(std::vector<Piece> pieces, Color player_to_move,
               std::array<Castling, 2> castling_rights,
               std::optional<int> en_passant_square, int halfmove_clock,
               int fullmove_number);

  bool operator==(const MailboxBoard &board) const;

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
  std::array<Square, 64> squares;
  GameState game_state;
  std::vector<GameState> history;

  Piece &get_piece(Piece piece);
  void remove_piece(Piece piece);
  void move_piece(Square &from, Square &to);
  void update_castling_rights(const Move &move);
  Move get_castling_rook_move(const Move &move) const;

  void make_normal(const Move &move);
  void make_en_passant(const Move &move);
  void make_pawn_two_squares_forward(const Move &move);
  void make_castling(const Move &move);
  void make_promotion(const Move &move);

  int get_psqt_score(const Piece &piece) const;
  bool is_lone_king(Color color) const;
  bool is_endgame() const;

  bool is_insufficient_material() const;
  bool is_threefold_repetition() const;
  bool is_draw_by_fifty_move_rule() const;

  int get_king_square(Color color) const;
  bool is_valid_en_passant(int capture_pos) const;
  std::vector<Move> get_pawn_captures(const Piece &piece) const;
  std::vector<Move> get_king_normal_moves(const Piece &piece,
                                          MoveCategory move_category) const;
  bool is_clear_path_castling(const Move &castling_move) const;
  std::vector<Move> get_potential_castling_moves() const;
  std::vector<Move>
  get_pseudo_legal_moves_direction(const Piece &piece, int x_direction,
                                   int y_direction,
                                   MoveCategory move_category) const;
  std::vector<Move>
  get_bishop_pseudo_legal_moves(const Piece &piece,
                                MoveCategory move_category) const;
  std::vector<Move>
  get_rook_pseudo_legal_moves(const Piece &piece,
                              MoveCategory move_category) const;
  std::vector<Move>
  get_queen_pseudo_legal_moves(const Piece &piece,
                               MoveCategory move_category) const;
  std::vector<Move>
  get_knight_pseudo_legal_moves(const Piece &piece,
                                MoveCategory move_category) const;
  std::vector<Move>
  get_pawn_pseudo_legal_moves(const Piece &piece,
                              MoveCategory move_category) const;
  bool is_attacked_by(int pos, Color color) const;
  bool passes_through_check_when_castling(const Move &castling_move) const;
  bool is_valid_castling(const Move &move) const;
  std::vector<Move> get_castling_moves() const;
  std::vector<Move>
  get_king_pseudo_legal_moves(const Piece &piece,
                              MoveCategory move_category) const;
  std::vector<Move> get_pseudo_legal_moves(const Piece &piece,
                                           MoveCategory move_category) const;
};
