#ifndef SQUARE_H
#define SQUARE_H

#include <optional>

#include "piece.hpp"


class Square {
    public:
        int x;
        int y;
        std::optional<Piece> piece;

        Square() {}
        Square(int x, int y);
        Square(int x, int y, std::optional<Piece> piece);
};

#endif
