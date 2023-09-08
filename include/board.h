#ifndef BOARD_H
#define BOARD_H

#include "square.h"

class Board {
    private:
        Square squares[8][8];
        Color player_to_move;

    public:
        Square get_square(int x, int y) { 
            return squares[y][x]; 
        }
        Color get_player_to_move() { 
            return player_to_move; 
        }
        void set_player_to_move(Color player_to_move) { 
            this->player_to_move = player_to_move; 
        }

        void setup_board(Board* board);
        void show_board(const Board* board);
        void deallocate_board(Board* board);
        Board copy_board(const Board* board);
        Board* get_position_from_fen(char* fen);
};

#endif
