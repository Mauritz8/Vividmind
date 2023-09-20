#include "square.h"

Square::Square(int x, int y) {
    this->x = x;
    this->y = y;
    this->piece = {};
}
Square::Square(int x, int y, const Piece& piece) : piece(piece) {
    this->x = x;
    this->y = y;
}

int Square::get_x() const {
    return x;
}
int Square::get_y() const {
    return y;
}
std::optional<Piece> Square::get_piece() const {
    return piece;
}
void Square::set_piece(const std::optional<Piece>& piece) {
    this->piece = piece;
}

bool Square::operator==(const Square& square) const {
    return x == square.x && y == square.y;
}


bool Square::is_outside_board() const {
    return this->get_x() < 0 || this->get_x() > 7 || this->get_y() < 0 || this->get_y() > 7;
}
