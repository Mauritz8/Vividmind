#include "engine/engine.h"

#include <chrono>
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
    int alpha = -200000;
    int beta = -alpha;
    Move best_move;
    std::vector<Move> legal_moves = move_gen.get_legal_moves(false);
    for (Move& move : legal_moves) {
        board_helper.make_appropriate(move);
        const int evaluation = -search(depth - 1, -beta, -alpha);
        board_helper.undo_appropriate();

        if (evaluation > alpha) {
            alpha = evaluation;
            best_move = move;
        }
    }
    evaluation = alpha;
    return best_move;
}

Move Engine::iterative_deepening_search(int max_depth) {
    current_depth = 0;
    nodes_searched = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    Move best_move;
    for (int depth = 1; depth <= max_depth; depth++) {
        current_depth = depth;
        best_move = get_best_move(depth); 
        auto stop_time = std::chrono::high_resolution_clock::now();
        time = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        show_uci_info();
    }
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

int Engine::evaluate() {
    nodes_searched++;

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

void Engine::show_uci_info() const {
    std::cout << "info";
    std::cout << " depth " << current_depth;
    std::cout << " score cp " << evaluation;
    std::cout << " nodes " << nodes_searched;
    std::cout << " time " << time;
    std::cout << "\n";
}
