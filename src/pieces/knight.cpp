#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/knight.h"
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>


std::vector<Move> get_knight_psuedo_legal_moves(const Square& start, Board& board) {
    const std::array<Pos, 8> end_squares = {
        Pos{start.x + 1, start.y + 2},
        Pos{start.x + 1, start.y - 2},
        Pos{start.x - 1, start.y + 2},
        Pos{start.x - 1, start.y - 2},
        Pos{start.x + 2, start.y + 1},
        Pos{start.x + 2, start.y - 1},
        Pos{start.x - 2, start.y + 1},
        Pos{start.x - 2, start.y - 1}
    };

    std::vector<Move> moves;
    for (Pos end : end_squares) {
        if (!is_outside_board(end)) {
            Square& end_square = board.get_square(end.x, end.y);
            moves.push_back(Move(start, end_square));
        }
    }
    return moves;
}
