#ifndef PIECE_H
#define PIECE_H

#include "pos.h"
#include <array>
#include <memory>
#include <optional>
#include <vector>

typedef enum {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING} Piece_type;
typedef enum {WHITE, BLACK} Color;

class Move;
class Board;
class Square;
class Piece {
    public:
        Piece_type piece_type;
        Color color;
        int x;
        int y;

        Piece(Piece_type piece_type, Color color, int x, int y);

        bool operator==(Piece piece) const;

        std::vector<Move> get_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_threatened_moves(Board& board);
        char get_char_representation() const;
        int get_value() const;
        std::array<std::array<int, 8>, 8> get_psqt() const;
};

std::vector<Move> get_psuedo_legal_moves_direction(const Square& start, int x_direction, int y_direction, Board& board);
char get_char_representation(Piece_type piece_type);
std::optional<Piece_type> get_piece_type(char char_representation);
std::optional<Piece_type> get_promotion_piece_type(char char_representation_lowercase);

#endif
