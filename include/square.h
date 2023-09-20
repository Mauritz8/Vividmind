#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <optional>

class Square {
    public:
        Square() {}
        Square(int x, int y);
        Square(int x, int y, const Piece& piece);

        int get_x() const;
        int get_y() const;
        std::optional<Piece> get_piece() const;
        void set_piece(const std::optional<Piece>& piece);

        bool operator==(const Square& square) const;

        bool is_outside_board() const;

    private:
        int x;
        int y;
        std::optional<Piece> piece;
};

#endif
