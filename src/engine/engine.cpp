#include "engine/engine.h"

#include <iostream>
#include <limits.h>
#include <optional>
#include <vector>

#include "move.h"


Engine::Engine(Board& board) 
    : board(board)
    , move_gen(board)
    , game_over_detector(board, move_gen)
{}

Move Engine::get_best_move(int depth) {
    int max = INT_MIN;
    std::vector<Move> legal_moves = move_gen.get_legal_moves(false);
    Move* best_move = nullptr;
    for (Move& move : legal_moves) {
        move.make_appropriate(board);
        const int evaluation = -search(depth - 1, -100000, 100000);
        move.undo_appropriate(board);
        if (evaluation > max) {
            max = evaluation;
            best_move = &move;
        }
    }
    std::cout << "info score cp " << max << "\n";
    return *best_move;
}

void Engine::divide(int depth) {
    move_gen.divide(depth);
}

int Engine::search(int depth, int alpha, int beta) {
    std::vector<Move> legal_moves = move_gen.get_legal_moves(false);

    if (game_over_detector.is_checkmate(legal_moves)) {
        return -KING_VALUE - depth;
    }
    if (game_over_detector.is_draw(legal_moves)) {
        return 0;
    }

    if (depth == 0) {
        return search_captures(alpha, beta);
    }

    for (Move& move : legal_moves) {
        move.make_appropriate(board);
        const int evaluation = -search(depth - 1, -beta, -alpha);
        move.undo_appropriate(board);

        if (evaluation >= beta) {
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
        }
    }
    return alpha;
}

int Engine::search_captures(int alpha, int beta) {
    int evaluation = evaluate();
    if (evaluation >= beta) {
        return beta;
    }
    if (evaluation > alpha) {
        alpha = evaluation;
    }


    std::vector<Move> captures = move_gen.get_legal_moves(true);
    for (Move& capture : captures) {
        capture.make_appropriate(board);
        evaluation = -search_captures(-beta, -alpha);
        capture.undo_appropriate(board);

        if (evaluation >= beta) {
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
        }
    }

    return alpha;
}

int Engine::evaluate() const {
    int evaluation = 0;
    const int white_material = board.game_state.material[WHITE];
    const int black_material = board.game_state.material[BLACK];
    evaluation += white_material - black_material;

    const int white_psqt = board.game_state.psqt[WHITE];
    const int black_psqt = board.game_state.psqt[BLACK];
    evaluation += white_psqt - black_psqt;

    if (board.game_state.player_to_move == BLACK) {
        return -evaluation;
    }
    return evaluation;
}
