#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "board.h"
#include <vector>

#define KING_SCORE 200
#define QUEEN_SCORE 9
#define ROOK_SCORE 5
#define BISHOP_SCORE 3
#define KNIGHT_SCORE 3
#define PAWN_SCORE 1

typedef struct {
    int kings;
    int queens;
    int rooks;
    int bishops;
    int knights;
    int pawns;
} PieceCounts;

Move get_best_move(int depth, const Board& board, const std::vector<Move> move_history);

#endif
