#include "board.hpp"
#include "board/bits.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "utils.hpp"
#include <cassert>
#include <cstdint>

uint64_t Board::get_castling_check_not_allowed_bb(int start,
                                                  bool kingside) const {
  uint64_t bb = masks.squares.at(start);
  return bb |= kingside
                   ? masks.squares.at(start + 1) | masks.squares.at(start + 2)
                   : masks.squares.at(start - 1) | masks.squares.at(start - 2);
}

uint64_t Board::get_castling_pieces_not_allowed_bb(int start,
                                                   bool kingside) const {
  return kingside ? masks.squares.at(start + 1) | masks.squares.at(start + 2)
                  : masks.squares.at(start - 1) | masks.squares.at(start - 2) |
                        masks.squares.at(start - 3);
}

uint64_t Board::gen_castling_moves_bb(int start) const {
  uint64_t castling = 0;

  int king_initial = get_player_to_move() == WHITE ? 60 : 4;
  if (start != king_initial) {
    return 0;
  }

  if (is_in_check(get_player_to_move())) {
    return 0;
  }

  Castling castling_rights =
      history.top().castling_rights.at(get_player_to_move());

  uint64_t attacked_bb =
      castling_rights.kingside || castling_rights.queenside
          ? get_attacking_bb(get_opponent(get_player_to_move()))
          : 0;

  uint64_t pieces_bb = (side_bbs.at(get_player_to_move()) &
                        ~piece_bbs.at(get_player_to_move()).at(KING)) |
                       side_bbs.at(get_opponent(get_player_to_move()));

  if (castling_rights.kingside) {
    uint64_t no_check_bb = get_castling_check_not_allowed_bb(start, true);
    uint64_t no_pieces_bb = get_castling_pieces_not_allowed_bb(start, true);
    if (((no_check_bb & attacked_bb) | (no_pieces_bb & pieces_bb)) == 0) {
      castling |= masks.squares.at(start + 2);
    }
  }

  if (castling_rights.queenside) {
    uint64_t no_check_bb = get_castling_check_not_allowed_bb(start, false);
    uint64_t no_pieces_bb = get_castling_pieces_not_allowed_bb(start, false);
    if (((no_check_bb & attacked_bb) | (no_pieces_bb & pieces_bb)) == 0) {
      castling |= masks.squares.at(start - 2);
    }
  }

  return castling;
}

void Board::gen_king_moves(int start, std::vector<Move> &moves) const {
  uint64_t normal =
      masks.king_moves.at(start) & ~side_bbs.at(get_player_to_move());
  while (normal != 0) {
    int end_pos = bits::pop_lsb(normal);
    Move move(start, end_pos);
    moves.push_back(move);
  }

  uint64_t castling = gen_castling_moves_bb(start);
  while (castling != 0) {
    int end_pos = bits::pop_lsb(castling);
    Move move(start, end_pos, CASTLING);
    moves.push_back(move);
  }
}

void Board::gen_pawn_moves(int start, std::vector<Move> &moves) const {
  uint64_t pawn = masks.squares.at(start);
  uint64_t all_pieces = side_bbs.at(WHITE) | side_bbs.at(BLACK);
  uint64_t all_pieces_one_rank_forward =
      get_player_to_move() == WHITE ? all_pieces >> 8 : all_pieces << 8;

  uint64_t move_one =
      masks.pawn_moves_one.at(get_player_to_move()).at(start) & ~all_pieces;
  uint64_t move_two = masks.pawn_moves_two.at(get_player_to_move()).at(start) &
                      ~(all_pieces | all_pieces_one_rank_forward);

  uint64_t captures = masks.pawn_captures.at(get_player_to_move()).at(start) &
                      ~side_bbs.at(get_player_to_move());
  uint64_t normal_captures =
      captures & side_bbs.at(get_opponent(get_player_to_move()));

  uint64_t en_passant_captures =
      get_en_passant_square().has_value()
          ? captures & masks.squares.at(get_en_passant_square().value())
          : 0;

  while (move_one != 0) {
    int end_pos = bits::pop_lsb(move_one);
    bool is_promotion = end_pos < 8 || end_pos > 55;
    if (is_promotion) {
      std::array<PieceType, 4> promotion_pieces = {
          QUEEN,
          ROOK,
          BISHOP,
          KNIGHT,
      };
      for (PieceType p : promotion_pieces) {
        Move move(start, end_pos, p);
        moves.push_back(move);
      }
    } else {
      Move move(start, end_pos);
      moves.push_back(move);
    }
  }

  while (normal_captures != 0) {
    int end_pos = bits::pop_lsb(normal_captures);
    bool is_promotion = end_pos < 8 || end_pos > 55;
    if (is_promotion) {
      std::array<PieceType, 4> promotion_pieces = {
          QUEEN,
          ROOK,
          BISHOP,
          KNIGHT,
      };
      for (PieceType p : promotion_pieces) {
        Move move(start, end_pos, p);
        moves.push_back(move);
      }
    } else {
      Move move(start, end_pos);
      moves.push_back(move);
    }
  }

  while (move_two != 0) {
    int end_pos = bits::pop_lsb(move_two);
    Move move(start, end_pos, PAWN_TWO_SQUARES_FORWARD);
    moves.push_back(move);
  }

  while (en_passant_captures != 0) {
    int end_pos = bits::pop_lsb(en_passant_captures);
    Move move(start, end_pos, EN_PASSANT);
    moves.push_back(move);
  }
}

// https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks#Sliding_Attacks_by_Calculation
// TODO: find a way to not use bits::reverse
uint64_t Board::gen_rank_attacks(int start) const {
  uint64_t o = side_bbs[WHITE] | side_bbs[BLACK];
  uint64_t s = masks.squares[start];
  uint64_t o_rev = bits::reverse(o);
  uint64_t s_rev = bits::reverse(s);
  return ((o - (s << 1)) ^ bits::reverse(o_rev - (s_rev << 1))) &
         masks.ranks[start >> 3];
}

uint64_t Board::gen_diag_attacks(int start, uint64_t diagonal_mask) const {
  uint64_t slider = masks.squares[start];     // single bit 1 << sq, 2^sq
  uint64_t lineMask = diagonal_mask ^ slider; // excludes square of slider

  uint64_t occ = side_bbs[WHITE] | side_bbs[BLACK];
  uint64_t forward =
      occ &
      lineMask; // also performs the first subtraction by clearing the s in o
  uint64_t reverse = std::byteswap(forward); // o'-s'
  forward -= slider;                         // o -2s
  reverse -= std::byteswap(slider);          // o'-2s'
  forward ^= std::byteswap(reverse);
  return forward & lineMask;
}

uint64_t Board::gen_rook_attacks(int start) const {
  return gen_diag_attacks(start, masks.files.at(start % 8)) |
         gen_rank_attacks(start);
}

uint64_t Board::gen_bishop_attacks(int start) const {
  int file = start % 8;
  int rank = start >> 3;
  return gen_diag_attacks(start, masks.diags.at(file + rank)) |
         gen_diag_attacks(start, masks.antidiags.at(file - rank + 7));
}

uint64_t Board::gen_queen_attacks(int start) const {
  return gen_rook_attacks(start) | gen_bishop_attacks(start);
}

void Board::gen_moves_piece(PieceType piece, int start,
                            std::vector<Move> &moves) const {
  if (piece == KING) {
    gen_king_moves(start, moves);
    return;
  }
  if (piece == PAWN) {
    gen_pawn_moves(start, moves);
    return;
  }

  uint64_t attacks = piece == KNIGHT   ? masks.knight_moves.at(start)
                     : piece == BISHOP ? gen_bishop_attacks(start)
                     : piece == ROOK   ? gen_rook_attacks(start)
                                       : gen_queen_attacks(start);

  uint64_t moves_bb = attacks &= ~side_bbs.at(get_player_to_move());
  while (moves_bb != 0) {
    int end_pos = bits::pop_lsb(moves_bb);
    Move move(start, end_pos);
    moves.push_back(move);
  }
}

void Board::gen_all_moves_piece(PieceType piece,
                                std::vector<Move> &moves) const {
  uint64_t piece_bb = piece_bbs.at(get_player_to_move()).at(piece);
  while (piece_bb != 0) {
    int start_pos = bits::pop_lsb(piece_bb);
    gen_moves_piece(piece, start_pos, moves);
  }
}

std::vector<Move> Board::get_pseudo_legal_moves() const {
  std::vector<Move> moves;
  for (int piece = 0; piece < 6; piece++) {
    gen_all_moves_piece((PieceType)piece, moves);
  }
  return moves;
}

std::vector<Move> Board::get_legal_moves() {
  const Color player = get_player_to_move();
  std::vector<Move> moves = get_pseudo_legal_moves();

  auto in_check_after_move = [this, player](const Move &move) {
    make(move);
    bool in_check = is_in_check(player);
    undo();
    return in_check;
  };

  std::erase_if(moves, in_check_after_move);
  return moves;
}

std::vector<Move> Board::get_forcing_moves() {
  const Color player = get_player_to_move();
  std::vector<Move> moves = get_legal_moves();

  auto not_forcing_move = [this, player](const Move &move) {
    make(move);
    bool is_forcing_move = get_captured_piece().has_value() ||
                           is_in_check(get_opponent(player)) ||
                           move.move_type == MoveType::PROMOTION;
    undo();
    return !is_forcing_move;
  };

  std::erase_if(moves, not_forcing_move);
  return moves;
}

uint64_t Board::get_attacking_bb(Color color) const {
  uint64_t attacking = 0;

  std::array<PieceType, 3> non_sliding_pieces = {KING, PAWN, KNIGHT};
  for (PieceType piece : non_sliding_pieces) {
    std::array<uint64_t, 64> piece_attacking_bb =
        piece == KING   ? masks.king_moves
        : piece == PAWN ? masks.pawn_captures.at(color)
                        : masks.knight_moves;
    uint64_t piece_bb = piece_bbs.at(color).at(piece);
    while (piece_bb != 0) {
      int start_pos = bits::pop_lsb(piece_bb);
      attacking |= piece_attacking_bb.at(start_pos);
    }
  }

  std::array<PieceType, 3> sliding_pieces = {BISHOP, ROOK, QUEEN};
  for (PieceType piece : sliding_pieces) {
    uint64_t piece_bb = piece_bbs.at(color).at(piece);
    while (piece_bb != 0) {
      int start_pos = bits::pop_lsb(piece_bb);
      attacking |= piece == BISHOP ? gen_bishop_attacks(start_pos)
                   : piece == ROOK ? gen_rook_attacks(start_pos)
                                   : gen_queen_attacks(start_pos);
    }
  }

  return attacking;
}

bool Board::is_attacking(int pos, Color color) const {
  std::array<uint64_t, 6> pieces_bb = piece_bbs.at(color);

  if ((masks.knight_moves.at(pos) & pieces_bb.at(KNIGHT)) != 0) {
    return true;
  }
  if ((masks.king_moves.at(pos) & pieces_bb.at(KING)) != 0) {
    return true;
  }
  if ((masks.pawn_captures.at(get_opponent(color)).at(pos) &
       pieces_bb.at(PAWN)) != 0) {
    return true;
  }

  uint64_t bishop_moves = gen_bishop_attacks(pos);
  if ((bishop_moves & pieces_bb.at(BISHOP)) != 0) {
    return true;
  }
  uint64_t rook_moves = gen_rook_attacks(pos);
  if ((rook_moves & pieces_bb.at(ROOK)) != 0) {
    return true;
  }
  if (((bishop_moves | rook_moves) & pieces_bb.at(QUEEN)) != 0) {
    return true;
  }

  return false;
}

bool Board::is_in_check(Color color) const {
  uint64_t king_bb = piece_bbs.at(color).at(KING);
  assert(king_bb != 0);
  int king_pos = bits::pop_lsb(king_bb);
  return is_attacking(king_pos, get_opponent(color));
}
