#include <functional>
#include <iostream>
#include <limits.h>
#include <memory>
#include <optional>
#include <vector>

#include "board.h"
#include "engine/engine.h"
#include "game_over_conditions.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include "pieces/pawn.h"


static double evaluate(const Board& board) {
    double score = 0;
    const int white_material = board.game_state.material[WHITE];
    const int black_material = board.game_state.material[BLACK];
    score += white_material - black_material;

    const int white_psqt = board.game_state.psqt[WHITE];
    const int black_psqt = board.game_state.psqt[BLACK];
    score += 0.01 * (white_psqt - black_psqt);

    if (board.game_state.player_to_move == BLACK) {
        return -score;
    }
    return score;
}

static double nega_max(int depth, Board& board) {
    if (depth == 0) {
        return evaluate(board);
    }

    std::vector<Move> legal_moves = get_legal_moves(board);
    if (is_checkmate(board, legal_moves)) {
        return -200;
    }
    if (is_draw(board, legal_moves)) {
        return 0;
    }

    double max = INT_MIN;
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate(board);
        const double score = -nega_max(depth - 1, board);
        move.undo_appropriate(board);
        if (score > max) {
            max = score;
        }
    }
    return max;
}

Move get_best_move(int depth, Board& board) {
    double max = INT_MIN;
    const std::vector<Move> legal_moves = get_legal_moves(board);
    const Move* best_move = nullptr;
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate(board);
        const double score = -nega_max(depth - 1, board);
        move.undo_appropriate(board);
        if (score > max) {
            max = score;
            best_move = &legal_moves.at(i);
        }
    }
    std::cout << "eval = " << max << "\n";
    return *best_move;
}


int perft(int depth, Board& board) {
    if (depth == 0) {
        return 1;
    }

    int nodes = 0;
    const std::vector<Move> move_list = get_legal_moves(board);
    for (int i = 0; i < move_list.size(); i++) {
        Move move = move_list.at(i);
        move.make_appropriate(board);
        nodes += perft(depth - 1, board);    
        move.undo_appropriate(board);
    }
    return nodes;
}

void divide(int depth, Board& board) {
    std::cout << "";
    int nodes_searched = 0;
    const std::vector<Move> move_list = get_legal_moves(board);
    for (int i = 0; i < move_list.size(); i++) {
        Move move = move_list.at(i);
        move.make_appropriate(board);
        const int nodes = perft(depth - 1, board);
        nodes_searched += nodes;
        std::cout << move.to_uci_notation() << ": " << nodes << "\n";
        move.undo_appropriate(board);
    }
    std::cout << "\nNodes searched: " << nodes_searched << "\n";
}
