#include "board.hpp"

#include <stdexcept>

#include "evaluation/evaluation.hpp"
#include "move.hpp"
#include "utils.hpp"

void Board::make(const Move &move) {
  game_state.next_move = move;
  history.push_back(game_state);

  game_state.en_passant_square = {};
  game_state.captured_piece = {};

  const Square &start = squares.at(move.start);
  if (start.piece->piece_type == PAWN) {
    game_state.halfmove_clock = 0;
  }
  switch (move.move_type) {
  case PAWN_TWO_SQUARES_FORWARD:
    make_pawn_two_squares_forward(move);
    break;
  case EN_PASSANT:
    make_en_passant(move);
    break;
  case PROMOTION:
    make_promotion(move);
    break;
  case CASTLING:
    make_castling(move);
    break;
  case NORMAL:
    make_normal(move);
    break;
  }

  update_castling_rights(move);

  if (game_state.player_to_move == BLACK) {
    game_state.fullmove_number++;
  }
  game_state.player_to_move = get_opposite_color(game_state.player_to_move);
}

void Board::undo() {
  Move move = history.back().next_move;
  Square &start = squares.at(move.start);
  Square &end = squares.at(move.end);

  if (move.move_type == CASTLING) {
    Move rook_move = get_castling_rook_move(move);
    move_piece(squares.at(rook_move.end), squares.at(rook_move.start));
  } else if (move.move_type == EN_PASSANT) {
    Piece captured_piece = *game_state.captured_piece;
    Square &captured_square = squares.at(captured_piece.pos);
    captured_square.piece = captured_piece;
    pieces.at(captured_piece.color).push_back(captured_piece);
    game_state.captured_piece = {};
  } else if (move.move_type == PROMOTION) {
    end.piece->piece_type = PAWN;
    Piece &piece_in_piece_list = get_piece(*end.piece);
    piece_in_piece_list.piece_type = PAWN;
  }

  move_piece(end, start);
  if (game_state.captured_piece) {
    end.piece = game_state.captured_piece;
    pieces.at(game_state.captured_piece->color)
        .push_back(*game_state.captured_piece);
  }

  game_state = history.at(history.size() - 1);
  history.pop_back();
}

void Board::make_normal(const Move &move) {
  Square &start = squares.at(move.start);
  Square &end = squares.at(move.end);

  if (end.piece) {
    game_state.halfmove_clock = 0;
    remove_piece(*end.piece);
  } else {
    game_state.halfmove_clock++;
  }
  move_piece(start, end);
}

void Board::make_castling(const Move &move) {
  Square &start = squares.at(move.start);
  Square &end = squares.at(move.end);

  Move rook_move = get_castling_rook_move(move);
  move_piece(squares.at(rook_move.start), squares.at(rook_move.end));
  move_piece(start, end);
}

void Board::make_pawn_two_squares_forward(const Move &move) {
  Square &start = squares.at(move.start);
  Square &end = squares.at(move.end);

  const int direction = start.piece->color == BLACK ? 1 : -1;
  game_state.en_passant_square = move.start + direction * 8;
  move_piece(start, end);
}

void Board::make_en_passant(const Move &move) {
  const int x_diff = move.end % 8 - move.start % 8;
  Square &captured_square = squares.at(move.start + x_diff);
  remove_piece(*captured_square.piece);
  captured_square.piece = {};

  Square &start = squares.at(move.start);
  Square &end = squares.at(move.end);
  move_piece(start, end);
}

void Board::make_promotion(const Move &move) {
  Square &start = squares.at(move.start);
  Square &end = squares.at(move.end);

  if (end.piece) {
    remove_piece(*end.piece);
  }

  Piece &piece = *start.piece;
  Piece &piece_in_game_state = get_piece(piece);
  PieceType promotion_piece = *move.promotion_piece;

  const int old_psqt = get_psqt_score(piece);
  piece.piece_type = promotion_piece;
  piece_in_game_state.piece_type = promotion_piece;
  const int new_value = piece.get_value();
  const int new_psqt = get_psqt_score(piece);
  game_state.material.at(piece.color) += new_value - PAWN_VALUE;
  game_state.psqt.at(piece.color) += new_psqt - old_psqt;
  move_piece(start, end);
}

Move Board::get_castling_rook_move(const Move &move) const {
  const int row = move.start / 8;
  const int move_direction = move.end - move.start;

  int start_x;
  int end_x;
  if (move_direction > 0) {
    // kingside castling
    start_x = 7;
    end_x = 5;
  } else {
    // queenside castling
    start_x = 0;
    end_x = 3;
  }
  Move rook_move = Move(start_x + row * 8, end_x + row * 8);
  return rook_move;
}

Piece &Board::get_piece(Piece piece) {
  for (Piece &p : pieces.at(piece.color)) {
    if (p.pos == piece.pos) {
      return p;
    }
  }
  throw std::invalid_argument("could not find piece");
}

void Board::remove_piece(Piece piece) {
  std::vector<Piece> &pieces = this->pieces.at(piece.color);
  for (auto it = pieces.begin(); it != pieces.end(); ++it) {
    if (*it == piece) {
      pieces.erase(it);
      game_state.material.at(piece.color) -= piece.get_value();
      game_state.psqt.at(piece.color) -= get_psqt_score(piece);
      game_state.captured_piece = piece;
      return;
    }
  }
}

void Board::move_piece(Square &start, Square &end) {
  Piece &piece_in_game_state = get_piece(*start.piece);
  piece_in_game_state.pos = end.pos;

  const int psqt_old = get_psqt_score(*start.piece);

  start.piece->pos = end.pos;
  end.piece = start.piece;
  start.piece = {};

  const int psqt_new = get_psqt_score(*end.piece);
  game_state.psqt.at(end.piece->color) += psqt_new - psqt_old;
}

void Board::update_castling_rights(const Move &move) {
  const Color color = game_state.player_to_move;
  const int player_starting_row = color == WHITE ? 7 : 0;
  const int opponent_starting_row = color == WHITE ? 0 : 7;
  const int player_king = player_starting_row * 8 + 4;
  const int player_rook1 = player_starting_row * 8 + 0;
  const int player_rook2 = player_starting_row * 8 + 7;
  const int opponent_rook1 = opponent_starting_row * 8 + 0;
  const int opponent_rook2 = opponent_starting_row * 8 + 7;

  if (move.start == player_king) {
    game_state.castling_rights.at(color).kingside = false;
    game_state.castling_rights.at(color).queenside = false;
  } else if (move.start == player_rook1) {
    game_state.castling_rights.at(color).queenside = false;
  } else if (move.start == player_rook2) {
    game_state.castling_rights.at(color).kingside = false;
  }

  if (move.end == opponent_rook1) {
    game_state.castling_rights.at(get_opposite_color(color)).queenside = false;
  } else if (move.end == opponent_rook2) {
    game_state.castling_rights.at(get_opposite_color(color)).kingside = false;
  }
}
