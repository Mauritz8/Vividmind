#include "bitboards_board.hpp"
#include "bits.hpp"
#include "defs.hpp"
#include "utils.hpp"

BitboardsBoard::BitboardsBoard(std::vector<Piece> pieces, Color player_to_move,
          std::array<Castling, 2> castling_rights,
          std::optional<int> en_passant_square, int halfmove_clock,
          int fullmove_number) {
}

BitboardsBoard BitboardsBoard::get_starting_position() {
}

bool BitboardsBoard::operator==(const BitboardsBoard &board) const {
}

const std::optional<Piece> &BitboardsBoard::get_piece(int pos) const {

}

Color BitboardsBoard::get_player_to_move() const {
  return game_state.player_to_move;
}

int BitboardsBoard::get_material(Color color) const {
}

int BitboardsBoard::get_psqt(Color color) const {
}

std::string BitboardsBoard::to_string() const {
}

void BitboardsBoard::make(const Move &move) {
}
void BitboardsBoard::undo() {
}

bool BitboardsBoard::is_draw() const {
}

bool BitboardsBoard::is_in_check(Color color) const {
}

std::vector<Move>
BitboardsBoard::get_pseudo_legal_moves(MoveCategory move_category) const {
}
