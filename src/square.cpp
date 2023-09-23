#include "square.h"
#include <algorithm>
#include <memory>

Square::Square(int x, int y) {
    this->x = x;
    this->y = y;
}

int Square::get_x() const {
    return x;
}
int Square::get_y() const {
    return y;
}
const std::unique_ptr<Piece>& Square::get_piece() const {
    return std::move(piece);
}
void Square::set_piece(std::unique_ptr<Piece>& piece) {
    this->piece = std::move(piece);
}

bool Square::operator==(const Square& square) const {
    return x == square.x && y == square.y;
}


bool Square::is_outside_board() const {
    return this->get_x() < 0 || this->get_x() > 7 || this->get_y() < 0 || this->get_y() > 7;
}

void Square::move_piece(Square& to) {
    to.set_piece(this->piece);
}
