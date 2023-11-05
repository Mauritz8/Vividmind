#ifndef MOVE_HPP
#define MOVE_HPP

#include <optional>
#include <string>
#include <vector>

#include "piece.hpp"
#include "square.hpp"


class Board;
class Move {
    public:
        int start;
        int end;
        bool is_castling_move;
        bool is_promotion;
        std::optional<PieceType> promotion_piece;
        bool is_en_passant;
        bool is_pawn_two_squares_forward;

        Move() {};
        Move(int start, int end);
        Move(const Square& start, const Square& end);
        Move(const Move& move);

        Move operator=(const Move& move);
        bool operator==(const Move& move) const;

        std::string to_uci_notation() const;
};

#endif
