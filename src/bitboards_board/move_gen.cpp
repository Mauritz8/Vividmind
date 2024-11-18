#include "bitboards_board.hpp"
#include "bitboards_board/bits.hpp"
#include "defs.hpp"
#include <sys/types.h>

std::vector<Move> BitboardsBoard::gen_all_king_moves() const {
  std::vector<Move> moves;

  u_int64_t bb_king = bb_pieces[pos_data.player_to_move][KING];
  std::optional<int> start_pos = bits::popLSB(bb_king);
  while (start_pos.has_value()) {
    std::vector<Move> pos_moves = gen_king_moves(start_pos.value());
    moves.insert(moves.end(), pos_moves.begin(), pos_moves.end()); 
    start_pos = bits::popLSB(bb_king);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_king_moves(int start) const {
  std::vector<Move> moves;

  u_int64_t king_moves = bbs.king_moves[start];
  for (int i = 0; i < 6; i++) {
    king_moves &= ~bb_pieces[pos_data.player_to_move][i];
  }

  std::optional<int> end_pos = bits::popLSB(king_moves);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(king_moves);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_all_knight_moves() const {
  std::vector<Move> moves;

  u_int64_t bb_knights = bb_pieces[pos_data.player_to_move][KNIGHT];
  std::optional<int> start_pos = bits::popLSB(bb_knights);
  while (start_pos.has_value()) {
    std::vector<Move> pos_moves = gen_knight_moves(start_pos.value());
    moves.insert(moves.end(), pos_moves.begin(), pos_moves.end()); 
    start_pos = bits::popLSB(bb_knights);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_knight_moves(int start) const {
  std::vector<Move> moves;

  u_int64_t knight_moves = bbs.knight_moves[start];
  for (int i = 0; i < 6; i++) {
    knight_moves &= ~bb_pieces[pos_data.player_to_move][i];
  }

  std::optional<int> end_pos = bits::popLSB(knight_moves);
  while (end_pos.has_value()) {
    Move move = Move(start, end_pos.value());
    moves.push_back(move);
    end_pos = bits::popLSB(knight_moves);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_all_pawn_moves() const {
  std::vector<Move> moves;

  u_int64_t bb_pawns = bb_pieces[pos_data.player_to_move][PAWN];
  std::optional<int> start_pos = bits::popLSB(bb_pawns);
  while (start_pos.has_value()) {
    std::vector<Move> pos_moves = gen_pawn_moves(start_pos.value());
    moves.insert(moves.end(), pos_moves.begin(), pos_moves.end()); 
    start_pos = bits::popLSB(bb_pawns);
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_pawn_moves(int start) const {
  std::vector<Move> moves;

  u_int64_t pawn_moves_straight =
      bbs.pawn_moves_straight[pos_data.player_to_move][start];
  for (int color = 0; color < 2; color++) {
    for (int piece = 0; piece < 6; piece++) {
      pawn_moves_straight &= ~bb_pieces[color][piece];
    }
  }

  u_int64_t bb_end = pawn_moves_straight;
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

  std::vector<Move> king_moves = gen_all_king_moves();
  moves.insert(moves.end(), king_moves.begin(), king_moves.end());

  std::vector<Move> knight_moves = gen_all_knight_moves();
  moves.insert(moves.end(), knight_moves.begin(), knight_moves.end());

  std::vector<Move> pawn_moves = gen_all_pawn_moves();
  moves.insert(moves.end(), pawn_moves.begin(), pawn_moves.end());

  return moves;
}
