#ifndef SQUARE_H
#define SQUARE_H

#include <optional>

#include "piece.hpp"


class Square {
    public:
        int pos;
        std::optional<Piece> piece;

        Square() {}
        Square(int pos);
        Square(int pos, std::optional<Piece> piece);
};

#endif
