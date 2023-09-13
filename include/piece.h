#ifndef PIECE_H
#define PIECE_H

#include <optional>

typedef enum {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING} Piece_type;
typedef enum {WHITE, BLACK} Color;

class Piece {
    public:
        Piece(Piece_type piece_type, Color color);

        Piece_type get_piece_type() const;
        void set_piece_type(Piece_type piece_type);
        Color get_color() const;
        void set_color(Color color);

    private:
        Piece_type piece_type;
        Color color;
};

char get_char_representation(Piece_type piece_type);
std::optional<Piece_type> get_piece_type(const char char_representation);
std::optional<Piece_type> get_promotion_piece_type(const char char_representation_lowercase);

#endif
