#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"
#include <optional>

class Square {
    private:
        int x;
        int y;
        std::optional<Piece> piece;

    public:
        Square() {}

        Square(int x, int y) {
            this->x = x;
            this->y = y;
            this->piece = {};
        }

        Square(int x, int y, Piece piece) : piece(piece) {
            this->x = x;
            this->y = y;
            this->piece = piece;
        }

        int get_x() const {
            return x;
        }
        int get_y() const {
            return y;
        }
        std::optional<Piece> get_piece() const {
            return piece;
        }
        void set_piece(std::optional<Piece> piece) {
            this->piece = piece;
        }

        bool operator==(const Square& square) const {
            return x == square.x && y == square.y;
        }
};

#endif
