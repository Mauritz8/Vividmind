#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "piece.h"
#include "square.h"
#include "board.h"

typedef struct {
    Square* start_square;
    Square* end_square;
    Piece* captured_piece;
    bool is_castling_move;
    bool is_promotion;
    Piece_type promotion_piece;
    bool is_en_passant;
} Move;

typedef struct {
    Move* moves;
    int capacity;
    int length;
} MoveArray;

void move_array_push(MoveArray* move_array, const Move* move);
MoveArray copy_move_array(const MoveArray* move_array);
bool validate_threatened_move(const Move* move, Board* board);
bool leaves_king_in_check(const Move* move, const Board* board);
bool is_promotion(const Move* move, Board* board);
bool is_legal_move(const Move* move, Board* board, const MoveArray* move_history);
void make_appropriate_move(Move* move, Board* board, MoveArray* move_history);
void undo_appropriate_move(Move* move, Board* board, MoveArray* move_history);
char* move_to_uci_notation(const Move* move);
Move uci_notation_to_move(const char* uci_notation, Board* board);

#endif
