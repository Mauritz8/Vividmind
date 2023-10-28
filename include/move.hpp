#ifndef MOVE_H
#define MOVE_H

#include <optional>
#include <string>
#include <vector>

#include "piece.hpp"
#include "pos.hpp"
#include "square.hpp"


class Board;
class Move {
    public:
        Pos start;
        Pos end;
        bool is_castling_move;
        bool is_promotion;
        std::optional<PieceType> promotion_piece;
        bool is_en_passant;
        bool is_pawn_two_squares_forward;

        Move() {};
        Move(int start_x, int start_y, int end_x, int end_y);
        Move(Pos start, Pos end);
        Move(const Square& start, const Square& end);
        static Move get_from_uci_notation(const std::string& uci_notation, Board& board);
        Move(const Move& move);

        Move operator=(const Move& move);
        bool operator==(const Move& move) const;

        std::string to_uci_notation() const;
};

#endif
