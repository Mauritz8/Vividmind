#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <memory>
#include <optional>


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
