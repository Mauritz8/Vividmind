#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <optional>

class Square {
    public:
        Square() {}
        Square(int x, int y);
        Square(int x, int y, Piece piece);

        int get_x() const;
        int get_y() const;
        std::optional<Piece> get_piece() const;
        void set_piece(std::optional<Piece> piece);

        bool operator==(const Square& square) const;

    private:
        int x;
        int y;
        std::optional<Piece> piece;
};

#endif
