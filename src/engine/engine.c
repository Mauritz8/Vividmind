#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "engine/engine.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"

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


static PieceCounts get_piece_counts(const Board* board, const Color color) {
    PieceCounts piece_counts = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Piece* piece = board->squares[i][j].piece;
            if (!piece || piece->color != color) {
                continue;
            }
            switch (piece->piece_type) {
                case KING:
                    piece_counts.kings++;
                    break;
                case QUEEN:
                    piece_counts.queens++;
                    break;
                case ROOK:
                    piece_counts.rooks++;
                    break;
                case BISHOP:
                    piece_counts.bishops++;
                    break;
                case KNIGHT:
                    piece_counts.knights++;
                    break;
                case PAWN:
                    piece_counts.pawns++;
                    break;
            }
        }
    }
    return piece_counts;
}

static int get_material_score(const Board* board) {
    const PieceCounts player_piece_counts = get_piece_counts(board, board->player_to_move);

    const Color opponent_color = get_opposite_color(board->player_to_move);
    const PieceCounts opponent_piece_counts = get_piece_counts(board, opponent_color);

    return KING_SCORE * (player_piece_counts.kings - opponent_piece_counts.kings) +
           QUEEN_SCORE * (player_piece_counts.queens - opponent_piece_counts.queens) +
           ROOK_SCORE * (player_piece_counts.rooks - opponent_piece_counts.rooks) +
           BISHOP_SCORE * (player_piece_counts.bishops - opponent_piece_counts.bishops) +
           KNIGHT_SCORE * (player_piece_counts.knights - opponent_piece_counts.knights) +
           PAWN_SCORE * (player_piece_counts.pawns - opponent_piece_counts.pawns);
}

static int evaluate(const Board* board) {
    const int material_score = get_material_score(board);
    const int score = material_score;
    return score;
}

static int nega_max(const int depth, const Board* board, const MoveArray* move_history) {
    Board board_copy = copy_board(board);
    MoveArray move_history_copy = copy_move_array(move_history);

    if (depth == 0) {
        return evaluate(&board_copy);
    }
    int max = INT_MIN;
    MoveArray legal_moves = get_all_legal_moves(&board_copy, &move_history_copy);
    for (int i = 0; i < legal_moves.length; i++) {
        const Move move = legal_moves.moves[i];
        make_appropriate_move(&move, &board_copy, &move_history_copy);
        const int score = -nega_max(depth - 1, &board_copy, &move_history_copy);
        board_copy = copy_board(board);
        move_history_copy = copy_move_array(move_history);
        if (score > max) {
            max = score;
        }
    }
    return max;
}

Move get_best_move(const int depth, const Board* board, const MoveArray* move_history) {
    Board board_copy = copy_board(board);
    MoveArray move_history_copy = copy_move_array(move_history);

    int max = INT_MIN;
    Move best_move;
    MoveArray legal_moves = get_all_legal_moves(&board_copy, &move_history_copy);
    for (int i = 0; i < legal_moves.length; i++) {
        Move move = legal_moves.moves[i];
        make_appropriate_move(&move, &board_copy, &move_history_copy);
        const int score = -nega_max(depth, &board_copy, &move_history_copy);
        board_copy = copy_board(board);
        move_history_copy = copy_move_array(move_history);
        if (score > max) {
            max = score;
            best_move = legal_moves.moves[i];
        }
    }
    return best_move;
}
