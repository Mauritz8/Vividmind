#ifndef SQUARE_HPP
#define SQUARE_HPP

#include <optional>

#include "defs.hpp"
#include "piece.hpp"


class Square {
    public:
        int pos;
        std::optional<Piece> piece;

        Square() {}
        Square(int pos);
        Square(int pos, std::optional<Piece> piece);

        bool is_occupied_by(Color color) const;
};

#endif
