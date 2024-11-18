#include "bitboards_board.hpp"
#include "bitboards_board/bits.hpp"
#include "defs.hpp"
#include <sys/types.h>

std::vector<Move> BitboardsBoard::gen_moves(
    u_int64_t bb_start_squares,
    std::array<u_int64_t, 64> bb_end_squares_lookup_table) const {
  std::vector<Move> moves;
  std::optional<int> start_pos = bits::pop(bb_start_squares);
  while (start_pos.has_value()) {
    u_int64_t bb_end_squares = bb_end_squares_lookup_table[start_pos.value()];
    for (int i = 0; i < 6; i++) {
      bb_end_squares &= ~bb_pieces[pos_data.player_to_move][i];
    }
    std::optional<int> end_pos = bits::pop(bb_end_squares);
    while (end_pos.has_value()) {
      Move move = Move(start_pos.value(), end_pos.value());
      moves.push_back(move);
      end_pos = bits::pop(bb_end_squares);
    }
    start_pos = bits::pop(bb_start_squares);
  }
  return moves;
}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
  std::vector<Move> moves;

  std::array<u_int64_t, 6> pieces = bb_pieces[pos_data.player_to_move];

  std::vector<Move> knight_moves =
      gen_moves(pieces[KNIGHT], move_gen_lookup_tables.bb_knight_moves);
  moves.insert(moves.end(), knight_moves.begin(), knight_moves.end());

  std::vector<Move> king_moves =
      gen_moves(pieces[KING], move_gen_lookup_tables.bb_king_moves);
  moves.insert(moves.end(), king_moves.begin(), king_moves.end());

  return moves;
}
