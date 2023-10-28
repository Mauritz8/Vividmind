#include "engine/engine.h"

#include <iostream>
#include <limits.h>
#include <optional>
#include <vector>

#include "board_helper.h"
#include "move.h"


Engine::Engine(Board& board, BoardHelper& board_helper) 
    : board(board)
    , board_helper(board_helper)
    , move_gen(board, board_helper)
    , game_over_detector(board, move_gen)
{}

Move Engine::get_best_move(int depth) {
    nodes_searched = 0;

    int alpha = -200000;
    int beta = -alpha;
    Move best_move;
    std::vector<Move> legal_moves = move_gen.get_legal_moves(false);
    for (Move& move : legal_moves) {
        board_helper.make_appropriate(move);
        const int evaluation = -search(depth - 1, alpha, beta);
        board_helper.undo_appropriate();

        if (evaluation > alpha) {
            alpha = evaluation;
            best_move = move;
        }
    }
    std::cout << "nodes searched = " << nodes_searched << "\n";
    std::cout << "info score cp " << alpha << "\n";
    return best_move;
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
        nodes_searched++;
        return search_captures(alpha, beta);
    }

    for (Move& move : legal_moves) {
        board_helper.make_appropriate(move);
        const int evaluation = -search(depth - 1, -beta, -alpha);
        board_helper.undo_appropriate();

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
        board_helper.make_appropriate(capture);
        evaluation = -search_captures(-beta, -alpha);
        board_helper.undo_appropriate();

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
