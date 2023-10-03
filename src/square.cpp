#include "square.h"
#include <algorithm>
#include <memory>

Square::Square(int x, int y) {
    this->x = x;
    this->y = y;
}

Square::Square(const Square& square) {
    this->x = square.get_x();
    this->y = square.get_y();
    if (square.get_piece()) {
        this->piece = square.get_piece()->clone();
    } else {
        this->piece = nullptr;
    }
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
std::unique_ptr<Piece> Square::get_piece() {
    return std::move(this->piece);
}
void Square::set_piece(std::unique_ptr<Piece> piece) {
    this->piece = std::move(piece);
}

bool Square::operator==(const Square& square) const {
    return x == square.x && y == square.y;
}


bool Square::is_outside_board() const {
    return this->get_x() < 0 || this->get_x() > 7 || this->get_y() < 0 || this->get_y() > 7;
}

void Square::move_piece(Square& to) {
    to.set_piece(this->get_piece());
}
