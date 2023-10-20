#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <memory>
#include <optional>

class Square {
    public:
        int x;
        int y;
        std::shared_ptr<Piece> piece;

        Square() {}
        Square(int x, int y);
        Square(const Square& square);

        bool operator==(const Square& square) const;

        void move_piece(Square& to);
};

#endif
