#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <memory>

class Square {
    public:
        int x;
        int y;
        std::shared_ptr<Piece> piece;

        Square() {}
        Square(int x, int y);
        Square(int x, int y, std::shared_ptr<Piece> piece);

        void move_piece(Square& to);
};

#endif
