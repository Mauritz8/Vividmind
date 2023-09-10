#include <stdlib.h>

#include "piece.h"
#include "test_utils.h"
#include "move.h"

void place_piece_at(const Piece& piece, Board& board, int x, int y) {
    board.get_square(x, y).piece.value() = piece;
}
