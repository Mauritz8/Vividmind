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
  Board(std::vector<Piece> pieces, Color player_to_move,
        std::array<Castling, 2> castling_rights,
        std::optional<int> en_passant_square, int halfmove_clock,
        int fullmove_number);

  static Board get_starting_position();

  bool operator==(const Board &board) const;

  const Square &get_square(int pos) const;
  const std::vector<Piece> &get_pieces(Color color) const;
  const Color get_player_to_move() const;
  const int get_material(Color color) const;
  const int get_psqt(Color color) const;

  std::string to_string() const;

  void make(const Move &move);
  void undo();

  bool is_draw() const;
  bool is_in_check(Color color) const;

  std::vector<Move> get_pseudo_legal_moves(MoveCategory move_category) const;

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
