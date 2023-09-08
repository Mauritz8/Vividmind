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
        Square(int x, int y, std::optional<Piece> piece) : piece(piece) {
            set_x(x);
            set_y(y);
            set_piece(piece);
        }

        int get_x() {
            return x;
        }
        void set_x(int x) {
            this->x = x;
        } 
        int get_y() {
            return y;
        }
        void set_y(int y) {
            this->y = y;
        }
        std::optional<Piece> get_piece() {
            return piece;
        }
        void set_piece(std::optional<Piece> piece) {
            this->piece = piece;
        }
};

#endif
