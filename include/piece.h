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
        Pos pos;

        Piece(Piece_type piece_type, Color color, Pos pos);

        bool operator==(Piece piece) const;

        std::vector<Move> get_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_threatened_moves(Board& board);
        char get_char_representation() const;
        int get_value() const;
        int get_psqt_score() const;

    private:
        std::vector<Move> get_bishop_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_rook_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_queen_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_knight_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_king_psuedo_legal_moves(Board& board) const;
        std::vector<Move> get_psuedo_legal_moves_direction(int x_direction, int y_direction, Board& board) const;
};

char get_char_representation(Piece_type piece_type);
std::optional<Piece_type> get_piece_type(char char_representation);
std::optional<Piece_type> get_promotion_piece_type(char char_representation_lowercase);

#endif
