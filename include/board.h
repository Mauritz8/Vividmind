#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include <string>
#include <vector>

class Board {
    public:
        Board() {}

        static Board get_empty_board();
        static Board get_starting_position();
        static Board get_position_from_fen(std::string fen);
        
        const Square& get_square(int x, int y) const;
        Square& get_square(int x, int y);
        void set_square(int x, int y, std::unique_ptr<Piece> piece);
        Color get_player_to_move() const;
        void set_player_to_move(Color player_to_move);

        void show() const;
        void switch_player_to_move();

    private:
        std::vector<std::vector<Square>> squares;
        Color player_to_move;

        void setup_pieces(Color color);
        void setup_pawns(Color color);
        void place_pieces(const std::string& fen_piece_placement_field);
        void set_player_to_move(const std::string& fen_active_color_field);
};

#endif
