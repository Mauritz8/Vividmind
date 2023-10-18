#ifndef PIECE_H
#define PIECE_H

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
        Piece(Color color, int x, int y);
        virtual ~Piece() = default;
        virtual std::unique_ptr<Piece> clone() const = 0;

        Color get_color() const;
        void set_color(Color color);
        int get_x() const;
        void set_x(int x);
        int get_y() const;
        void set_y(int y);

        virtual std::vector<Move> get_psuedo_legal_moves(const Board& board) const = 0;
        std::vector<Move> get_threatened_moves(const Board& board);
        virtual char get_char_representation() const = 0;
        virtual std::vector<std::vector<int>> get_piece_square_table() const = 0;
        virtual int get_value() const = 0;

    protected:
        std::vector<Move> get_psuedo_legal_moves_direction(const Square& start, int x_direction, int y_direction, const Board& board) const;

    private:
        Color color;
        int x;
        int y;
};

char get_char_representation(Piece_type piece_type);
std::optional<Piece_type> get_piece_type(char char_representation);
std::optional<Piece_type> get_promotion_piece_type(char char_representation_lowercase);
std::shared_ptr<Piece> create_piece(Piece_type piece_type, Color color, int x, int y);

#endif
