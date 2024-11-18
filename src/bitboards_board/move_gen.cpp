#include "bitboards_board.hpp"
#include "bitboards_board/bits.hpp"
#include "defs.hpp"
#include "utils.hpp"
#include <sys/types.h>

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

bool BitboardsBoard::piece_on_square(u_int64_t pos_bb, Color color) const {
  for (int piece = 0; piece < 6; piece++) {
    if ((pos_bb & ~bb_pieces.at(color).at(piece)) == 0) {
      return true;
    }
  }
  return false;
}

std::vector<Move> BitboardsBoard::gen_sliding_moves(int start, Direction direction) const {
  std::vector<Move> moves;

  u_int64_t pos_bb = bbs.squares[start];
  int pos = start;

  auto stop = [&]() {
    switch (direction) {
      case UP:
        return (pos_bb & ~bbs.rank_1) == 0;
      case DOWN:
        return (pos_bb & ~bbs.rank_8) == 0;
      case LEFT:
        return (pos_bb & ~bbs.a_file) == 0;
      case RIGHT:
        return (pos_bb & ~bbs.h_file) == 0;
      case UP_LEFT:
        return (pos_bb & ~(bbs.a_file | bbs.rank_1)) == 0;
      case UP_RIGHT:
        return (pos_bb & ~(bbs.h_file | bbs.rank_1)) == 0;
      case DOWN_LEFT:
        return (pos_bb & ~(bbs.a_file | bbs.rank_8)) == 0;
      case DOWN_RIGHT:
        return (pos_bb & ~(bbs.h_file | bbs.rank_8)) == 0;
    }
  };

  auto go_next_pos = [&]() {
    switch (direction) {
      case UP:
        pos_bb >>= 8;
        pos -= 8;
        break;
      case DOWN:
        pos_bb <<= 8;
        pos += 8;
        break;
      case LEFT:
        pos_bb >>= 1;
        pos -= 1;
        break;
      case RIGHT:
        pos_bb <<= 1;
        pos += 1;
        break;
      case UP_LEFT:
        pos_bb >>= 9;
        pos -= 9;
        break;
      case UP_RIGHT:
        pos_bb >>= 7;
        pos -= 7;
        break;
      case DOWN_LEFT:
        pos_bb <<= 7;
        pos += 7;
        break;
      case DOWN_RIGHT:
        pos_bb <<= 9;
        pos += 9;
        break;
    }
  };

  while (!stop()) {
    go_next_pos();
    if (piece_on_square(pos_bb, pos_data.player_to_move)) {
      break;
    }

    Move move = Move(start, pos);
    moves.push_back(move);

    if (piece_on_square(pos_bb, get_opposite_color(pos_data.player_to_move))) {
      break;
    }
  }

  return moves;
}

std::vector<Move> BitboardsBoard::gen_sliding_moves_directions(
    int start, std::vector<Direction> directions) const {
  std::vector<Move> moves;
  for (Direction direction : directions) {
    std::vector<Move> moves_direction = gen_sliding_moves(start, direction);
    moves.insert(moves.end(), moves_direction.begin(), moves_direction.end());
  }
  return moves;
}

std::vector<Move> BitboardsBoard::gen_rook_moves(int start) const {
  const std::vector<Direction> directions = {UP, DOWN, LEFT, RIGHT};
  return gen_sliding_moves_directions(start, directions);
}

std::vector<Move> BitboardsBoard::gen_bishop_moves(int start) const {
  const std::vector<Direction> directions = {UP_LEFT, UP_RIGHT, DOWN_LEFT,
                                         DOWN_RIGHT};
  return gen_sliding_moves_directions(start, directions);
}

std::vector<Move> BitboardsBoard::gen_queen_moves(int start) const {
  std::vector<Move> moves;

  std::vector<Move> rook = gen_rook_moves(start);
  moves.insert(moves.end(), rook.begin(), rook.end());
  std::vector<Move> bishop = gen_bishop_moves(start);
  moves.insert(moves.end(), bishop.begin(), bishop.end());

  return moves;
}

std::vector<Move> BitboardsBoard::gen_moves_piece(PieceType piece, int start) const {
  switch (piece) {
    case KING:
      return gen_king_moves(start);
    case KNIGHT:
      return gen_knight_moves(start);
    case PAWN:
      return gen_pawn_moves(start);
    case ROOK:
      return gen_rook_moves(start);
    case BISHOP:
      return gen_bishop_moves(start);
    case QUEEN:
      return gen_queen_moves(start);
  }
}

std::vector<Move> BitboardsBoard::gen_all_moves_piece(PieceType piece) const {
  std::vector<Move> moves;

  u_int64_t bb_piece = bb_pieces[pos_data.player_to_move][piece];
  std::optional<int> start_pos = bits::popLSB(bb_piece);
  while (start_pos.has_value()) {
    std::vector<Move> pos_moves = gen_moves_piece(piece, start_pos.value());
    moves.insert(moves.end(), pos_moves.begin(), pos_moves.end()); 
    start_pos = bits::popLSB(bb_piece);
  }
  return moves;
}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
  std::vector<Move> moves;
  for (int piece = 0; piece < 6; piece++) {
    std::vector<Move> piece_moves = gen_all_moves_piece((PieceType) piece);
    moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
  }
  return moves;
}
