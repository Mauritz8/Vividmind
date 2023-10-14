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
const std::shared_ptr<Piece>& Square::get_piece() const {
    return piece;
}
std::shared_ptr<Piece> Square::get_piece() {
    return this->piece;
}
void Square::set_piece(std::shared_ptr<Piece> piece) {
    this->piece = piece;
}

bool Square::operator==(const Square& square) const {
    return x == square.x && y == square.y;
}


void Square::move_piece(Square& to) {
    std::shared_ptr<Piece> piece = std::move(this->get_piece());
    piece->set_x(to.get_x());
    piece->set_y(to.get_y());
    to.set_piece(std::move(piece));
    this->set_piece(nullptr);
}
