#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include <string>

class Board {
    private:
        Square squares[8][8];
        Color player_to_move;

        void setup_pieces(int row, const Piece_type* order, Color color);
        void setup_pawns(int row, Color color);
        int place_pieces(const std::string fen_piece_placement_field);
        int set_player_to_move(const std::string fen_active_color_field);

    public:
        Board() {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    get_square(j, i) = Square(j, i);
                }
            }
        }

        Board(std::string fen);

        Square get_square(int x, int y) const { 
            return squares[y][x]; 
        }
        Color get_player_to_move() const { 
            return player_to_move; 
        }
        void set_player_to_move(Color player_to_move) { 
            this->player_to_move = player_to_move; 
        }

        void setup_board();
        void show_board();
        Board get_position_from_fen(char* fen);
};

#endif
