#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <memory>

class Square {
    public:
        Square() {}
        Square(int x, int y);

        int get_x() const;
        int get_y() const;
        const std::unique_ptr<Piece>& get_piece() const;
        void set_piece(std::unique_ptr<Piece>& piece);

        bool operator==(const Square& square) const;

        bool is_outside_board() const;
        void move_piece(Square& to);

    private:
        int x;
        int y;
        std::unique_ptr<Piece> piece;
};

#endif
