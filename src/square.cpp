#include "square.h"
#include <memory>


Square::Square(int x, int y) {
    this->x = x;
    this->y = y;
}

Square::Square(int x, int y, std::shared_ptr<Piece> piece) {
    this->x = x;
    this->y = y;
    this->piece = piece;
}

void Square::move_piece(Square& to) {
    piece->pos = Pos{to.x, to.y};
    to.piece = std::move(piece);
    piece = nullptr;
}
