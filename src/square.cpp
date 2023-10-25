#include "square.h"


Square::Square(int x, int y) {
    this->x = x;
    this->y = y;
}

Square::Square(int x, int y, std::optional<Piece> piece) {
    this->x = x;
    this->y = y;
    this->piece = piece;
}
