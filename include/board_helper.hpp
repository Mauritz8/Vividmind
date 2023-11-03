#ifndef BOARD_HELPER_H
#define BOARD_HELPER_H

#include "board.hpp"
#include "move.hpp"
#include "piece.hpp"


class BoardHelper {
    public:
        BoardHelper(Board& board);

        bool is_clear_line(int pos1, int pos2) const;
        bool is_clear_diagonal(int pos1, int pos2) const;
        int get_king_square(Color color) const;

    private:
        Board& board;
};

#endif
