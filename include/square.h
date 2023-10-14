#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <memory>

class Square {
    public:
        Square() {}
        Square(int x, int y);
        Square(const Square& square);

        int get_x() const;
        int get_y() const;
        const std::shared_ptr<Piece>& get_piece() const;
        std::shared_ptr<Piece> get_piece();
        void set_piece(std::shared_ptr<Piece> piece);

        bool operator==(const Square& square) const;

        void move_piece(Square& to);

    private:
        int x;
        int y;
        std::shared_ptr<Piece> piece;
};

#endif
