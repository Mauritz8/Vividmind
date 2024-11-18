#include "bitboards_board.hpp"
#include "bitboards_board/bits.hpp"
#include "defs.hpp"
#include <sys/types.h>

std::vector<Move>
BitboardsBoard::gen_moves_piece(u_int64_t bb_start,
                                std::array<u_int64_t, 64> bbs_end) const {
  std::vector<Move> moves;
  std::optional<int> start_pos = bits::popLSB(bb_start);
  while (start_pos.has_value()) {
    u_int64_t bb_end = bbs_end[start_pos.value()];
    std::vector<Move> pos_moves = gen_moves(start_pos.value(), bb_end);
    moves.insert(moves.end(), pos_moves.begin(), pos_moves.end()); 
    start_pos = bits::popLSB(bb_start);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_moves(int start, u_int64_t bb_end) const {
  std::vector<Move> moves;
  for (int i = 0; i < 6; i++) {
    bb_end &= ~bb_pieces[pos_data.player_to_move][i];
  }

  std::optional<int> end_pos = bits::popLSB(bb_end);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(bb_end);
  }

  return moves;
}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
  std::vector<Move> moves;

  std::array<u_int64_t, 6> pieces = bb_pieces[pos_data.player_to_move];

  std::vector<Move> knight_moves =
      gen_moves_piece(pieces[KNIGHT], move_gen_lookup_tables.bb_knight_moves);
  moves.insert(moves.end(), knight_moves.begin(), knight_moves.end());

  std::vector<Move> king_moves =
      gen_moves_piece(pieces[KING], move_gen_lookup_tables.bb_king_moves);
  moves.insert(moves.end(), king_moves.begin(), king_moves.end());

  return moves;
}
