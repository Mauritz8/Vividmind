#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "square.h"
#include "board.h"

typedef struct {
    Square* start_square;
    Square* end_square;
} Move;

typedef struct {
    Move* moves;
    int length;
} MoveArray;

void make_move(const Move* move, Board* board);
bool validate_move_basic(const Move* move, const Board* board);
bool validate_threatened_move(const Move* move, Board* board);
bool leaves_king_in_check(const Move* move, const Board* board);
bool is_castling_move(const Move* move);
void make_castling_move(const Move* move, Board* board);
bool is_legal_move(const Move* move, Board* board, const MoveArray* move_history);
char* move_to_uci_notation(const Move* move);
Move uci_notation_to_move(const char* uci_notation, Board* board);

#endif
