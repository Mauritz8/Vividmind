#include "mailbox_board.hpp"

#include <array>
#include <iostream>
#include <vector>

#include "mailbox_board/mailbox_board_defs.hpp"
#include "fmt/core.h"
#include "move.hpp"
#include "square.hpp"
#include "utils.hpp"

bool MailboxBoard::is_valid_en_passant(int capture_pos) const {
  if (!game_state.en_passant_square.has_value()) {
    return false;
  }
  return capture_pos == game_state.en_passant_square.value();
}

std::vector<Move> MailboxBoard::get_pawn_captures(const Piece &piece) const {
  const int direction = piece.color == BLACK ? 1 : -1;
  const std::array<int, 2> movements = {9 * direction, 11 * direction};

  std::vector<Move> moves;
  moves.reserve(2);
  for (int movement : movements) {
    const int end = mailbox.at(mailbox64.at(piece.pos) + movement);
    if (end == -1) {
      continue;
    }

    const Square &square = squares.at(end);
    if (square.piece && square.piece->color != piece.color) {
      moves.push_back(Move(piece.pos, end));
    } else if (is_valid_en_passant(end)) {
      Move move = Move(piece.pos, end);
      move.move_type = EN_PASSANT;
      moves.push_back(move);
    }
  }
  return moves;
}

std::vector<Move>
MailboxBoard::get_king_normal_moves(const Piece &piece,
                             MoveCategory move_category) const {
  std::array<int, 8> movements = {11, -11, 10, -10, 9, -9, 1, -1};
  const Color opponent = get_opposite_color(piece.color);
  std::vector<Move> moves;
  moves.reserve(8);
  for (int movement : movements) {
    const int end = mailbox.at(mailbox64.at(piece.pos) + movement);
    if (end == -1) {
      continue;
    }

    if (move_category == TACTICAL && squares.at(end).is_occupied_by(opponent)) {
      moves.push_back(Move(piece.pos, end));
    } else if (move_category == ALL &&
               !squares.at(end).is_occupied_by(piece.color)) {
      moves.push_back(Move(piece.pos, end));
    }
  }
  return moves;
}

bool MailboxBoard::is_clear_path_castling(const Move &castling_move) const {
  const int x_direction = castling_move.end - castling_move.start > 0 ? 1 : -1;
  const int rook_pos =
      x_direction == 1 ? castling_move.end + 1 : castling_move.end - 2;

  int pos = castling_move.start + x_direction;
  while (pos != rook_pos) {
    if (squares.at(pos).piece) {
      return false;
    }
    pos += x_direction;
  }
  return true;
}

std::vector<Move> MailboxBoard::get_potential_castling_moves() const {
  const Color color = game_state.player_to_move;
  const int king_start = color == BLACK ? 4 : 60;

  std::vector<Move> potential_castling_moves;
  potential_castling_moves.reserve(2);
  if (game_state.castling_rights.at(color).kingside) {
    const int kingside_end = color == BLACK ? 6 : 62;
    potential_castling_moves.push_back(Move(king_start, kingside_end));
  }

  if (game_state.castling_rights.at(color).queenside) {
    const int queenside_end = color == BLACK ? 2 : 58;
    potential_castling_moves.push_back(Move(king_start, queenside_end));
  }

  return potential_castling_moves;
}

std::vector<Move>
MailboxBoard::get_pseudo_legal_moves_direction(const Piece &piece, int x_direction,
                                        int y_direction,
                                        MoveCategory move_category) const {
  std::vector<Move> moves;

  const int step = x_direction + y_direction * 10;
  int pos = mailbox.at(mailbox64.at(piece.pos) + step);
  while (pos != -1) {
    const Square &end = squares.at(pos);
    if (end.piece) {
      if (end.piece->color != piece.color) {
        moves.push_back(Move(piece.pos, pos));
      }
      break;
    }

    if (move_category == ALL) {
      moves.push_back(Move(piece.pos, pos));
    }
    pos = mailbox.at(mailbox64.at(pos) + step);
  }

  return moves;
}

std::vector<Move>
MailboxBoard::get_bishop_pseudo_legal_moves(const Piece &piece,
                                     MoveCategory move_category) const {
  std::vector<Move> moves;

  std::vector<Move> up_right =
      get_pseudo_legal_moves_direction(piece, 1, 1, move_category);
  std::vector<Move> down_right =
      get_pseudo_legal_moves_direction(piece, 1, -1, move_category);
  std::vector<Move> down_left =
      get_pseudo_legal_moves_direction(piece, -1, -1, move_category);
  std::vector<Move> up_left =
      get_pseudo_legal_moves_direction(piece, -1, 1, move_category);

  moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
  moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
  moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
  moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));

  return moves;
}

std::vector<Move>
MailboxBoard::get_rook_pseudo_legal_moves(const Piece &piece,
                                   MoveCategory move_category) const {
  std::vector<Move> moves;

  std::vector<Move> up =
      get_pseudo_legal_moves_direction(piece, 0, 1, move_category);
  std::vector<Move> right =
      get_pseudo_legal_moves_direction(piece, 1, 0, move_category);
  std::vector<Move> down =
      get_pseudo_legal_moves_direction(piece, 0, -1, move_category);
  std::vector<Move> left =
      get_pseudo_legal_moves_direction(piece, -1, 0, move_category);

  moves.insert(std::end(moves), std::begin(up), std::end(up));
  moves.insert(std::end(moves), std::begin(right), std::end(right));
  moves.insert(std::end(moves), std::begin(down), std::end(down));
  moves.insert(std::end(moves), std::begin(left), std::end(left));

  return moves;
}

std::vector<Move>
MailboxBoard::get_queen_pseudo_legal_moves(const Piece &piece,
                                    MoveCategory move_category) const {
  std::vector<Move> moves;

  std::vector<Move> bishop_moves =
      get_bishop_pseudo_legal_moves(piece, move_category);
  std::vector<Move> rook_moves =
      get_rook_pseudo_legal_moves(piece, move_category);
  moves.insert(std::end(moves), std::begin(bishop_moves),
               std::end(bishop_moves));
  moves.insert(std::end(moves), std::begin(rook_moves), std::end(rook_moves));

  return moves;
}

std::vector<Move>
MailboxBoard::get_knight_pseudo_legal_moves(const Piece &piece,
                                     MoveCategory move_category) const {
  const std::array<int, 8> movements = {21, -21, 19, -19, 12, -12, 8, -8};
  const Color opponent = get_opposite_color(piece.color);
  std::vector<Move> moves;
  moves.reserve(8);
  for (int movement : movements) {
    const int end = mailbox.at(mailbox64.at(piece.pos) + movement);
    if (end == -1) {
      continue;
    }

    if (move_category == TACTICAL && squares.at(end).is_occupied_by(opponent)) {
      moves.push_back(Move(piece.pos, end));
    } else if (move_category == ALL &&
               !squares.at(end).is_occupied_by(piece.color)) {
      moves.push_back(Move(piece.pos, end));
    }
  }
  return moves;
}

std::vector<Move>
MailboxBoard::get_pawn_pseudo_legal_moves(const Piece &piece,
                                   MoveCategory move_category) const {
  std::vector<Move> moves;
  moves.reserve(4);
  const int direction = game_state.player_to_move == BLACK ? 1 : -1;
  int start = piece.pos;

  const int y = start / 8;
  const int about_to_promote_row = game_state.player_to_move == WHITE ? 1 : 6;
  const bool is_promotion_move = y == about_to_promote_row;
  if (move_category == ALL || move_category == TACTICAL && is_promotion_move) {
    const int end1 = start + 8 * direction;
    if (!squares.at(end1).piece) {
      moves.push_back(Move(start, end1));

      const int starting_row = piece.color == BLACK ? 1 : 6;
      if (start / 8 == starting_row) {
        const int end2 = start + 16 * direction;
        if (!squares.at(end2).piece) {
          Move move = Move(start, end2);
          move.move_type = PAWN_TWO_SQUARES_FORWARD;
          moves.push_back(move);
        }
      }
    }
  }

  const std::vector<Move> captures = get_pawn_captures(piece);
  moves.insert(moves.end(), captures.begin(), captures.end());

  if (is_promotion_move) {
    const std::array<PieceType, 3> other_promotion_piece_types = {ROOK, BISHOP,
                                                                  KNIGHT};
    std::vector<Move> other_promotion_moves;
    for (Move &move : moves) {
      move.move_type = PROMOTION;
      move.promotion_piece = QUEEN;
      for (PieceType piece_type : other_promotion_piece_types) {
        Move promotion_move = move;
        promotion_move.promotion_piece = piece_type;
        other_promotion_moves.push_back(promotion_move);
      }
    }
    moves.insert(moves.end(), other_promotion_moves.begin(),
                 other_promotion_moves.end());
  }

  return moves;
}

bool MailboxBoard::is_attacked_by(int pos, Color color) const {
  const Piece &piece = Piece(KING, get_opposite_color(color), pos);

  const std::vector<Move> &rook_moves =
      get_rook_pseudo_legal_moves(piece, TACTICAL);
  for (const Move &move : rook_moves) {
    const PieceType piece_type = squares.at(move.end).piece->piece_type;
    if (piece_type == ROOK || piece_type == QUEEN) {
      return true;
    }
  }

  const std::vector<Move> &bishop_moves =
      get_bishop_pseudo_legal_moves(piece, TACTICAL);
  for (const Move &move : bishop_moves) {
    const PieceType piece_type = squares.at(move.end).piece->piece_type;
    if (piece_type == BISHOP || piece_type == QUEEN) {
      return true;
    }
  }

  const std::vector<Move> &knight_moves =
      get_knight_pseudo_legal_moves(piece, TACTICAL);
  for (const Move &move : knight_moves) {
    if (squares.at(move.end).piece->piece_type == KNIGHT) {
      return true;
    }
  }

  const std::vector<Move> pawn_captures = get_pawn_captures(piece);
  for (const Move &move : pawn_captures) {
    if (squares.at(move.end).piece->piece_type == PAWN &&
        game_state.en_passant_square != move.end) {
      return true;
    }
  }

  const std::vector<Move> king_moves = get_king_normal_moves(piece, TACTICAL);
  for (const Move &move : king_moves) {
    if (squares.at(move.end).piece->piece_type == KING) {
      return true;
    }
  }
  return false;
}

bool MailboxBoard::passes_through_check_when_castling(
    const Move &castling_move) const {
  const int direction = castling_move.end - castling_move.start > 0 ? 1 : -1;
  const Color player = game_state.player_to_move;
  const Color opponent = get_opposite_color(player);

  int pos = castling_move.start + direction;
  while (true) {
    if (is_attacked_by(pos, opponent)) {
      return true;
    }
    if (pos == castling_move.end)
      break;
    pos += direction;
  }
  return false;
}

bool MailboxBoard::is_valid_castling(const Move &move) const {
  if (!is_clear_path_castling(move)) {
    return false;
  }
  if (is_in_check(game_state.player_to_move)) {
    return false;
  }
  if (passes_through_check_when_castling(move)) {
    return false;
  }

  return true;
}

std::vector<Move> MailboxBoard::get_castling_moves() const {
  std::vector<Move> castling_moves = get_potential_castling_moves();
  for (auto it = castling_moves.begin(); it != castling_moves.end();) {
    if (!is_valid_castling(*it)) {
      it = castling_moves.erase(it);
    } else {
      it->move_type = CASTLING;
      ++it;
    }
  }
  return castling_moves;
}

std::vector<Move>
MailboxBoard::get_king_pseudo_legal_moves(const Piece &piece,
                                   MoveCategory move_category) const {
  std::vector<Move> moves = get_king_normal_moves(piece, move_category);
  moves.reserve(10);
  if (move_category == ALL) {
    std::vector<Move> castling_moves = get_castling_moves();
    moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
  }
  return moves;
}

std::vector<Move>
MailboxBoard::get_pseudo_legal_moves(const Piece &piece,
                              MoveCategory move_category) const {
  switch (piece.piece_type) {
  case KING: {
    return get_king_pseudo_legal_moves(piece, move_category);
  }
  case QUEEN: {
    return get_queen_pseudo_legal_moves(piece, move_category);
  }
  case ROOK: {
    return get_rook_pseudo_legal_moves(piece, move_category);
  }
  case BISHOP: {
    return get_bishop_pseudo_legal_moves(piece, move_category);
  }
  case KNIGHT: {
    return get_knight_pseudo_legal_moves(piece, move_category);
  }
  case PAWN: {
    return get_pawn_pseudo_legal_moves(piece, move_category);
  }
  }
  return {};
}

std::vector<Move>
MailboxBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
  const std::vector<Piece> &pieces =
      game_state.pieces.at(game_state.player_to_move);
  std::vector<Move> moves;
  for (Piece piece : pieces) {
    std::vector<Move> piece_moves =
        get_pseudo_legal_moves(piece, move_category);
    moves.insert(std::end(moves), std::begin(piece_moves),
                 std::end(piece_moves));
  }
  return moves;
}

bool MailboxBoard::is_in_check(Color color) const {
  return is_attacked_by(get_king_square(color), get_opposite_color(color));
}
