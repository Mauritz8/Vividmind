#include "square.h"
#include <algorithm>
#include <memory>

Square::Square(int x, int y) {
    this->x = x;
    this->y = y;
}

Square::Square(const Square& square) {
    x = square.x;
    y = square.y;
    piece = square.piece;
}

bool Square::operator==(const Square& square) const {
    return x == square.x && y == square.y;
}


void Square::move_piece(Square& to) {
    piece->pos = Pos{to.x, to.y};
    to.piece = std::move(piece);
    piece = nullptr;
}
