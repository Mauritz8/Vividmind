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
            set_x(x);
            set_y(y);
        }

        Square(int x, int y, Piece piece) : piece(piece) {
            Square(x, y);
            this->piece.value() = piece;
        }

        int get_x() const {
            return x;
        }
        void set_x(int x) {
            this->x = x;
        } 
        int get_y() const {
            return y;
        }
        void set_y(int y) {
            this->y = y;
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
