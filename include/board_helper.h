#ifndef BOARD_HELPER_H
#define BOARD_HELPER_H

#include "board.h"
#include "piece.h"
#include "pos.h"


class BoardHelper {
    public:
        BoardHelper(const Board& board);

        bool is_occupied_by_color(Pos pos, Color color) const;
        bool is_clear_line(Pos pos1, Pos pos2) const;
        bool is_clear_diagonal(Pos pos1, Pos pos2) const;
        Pos get_king_square(Color color) const;

    private:
        const Board& board;
};

#endif
