#include "engine/engine.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits.h>
#include <optional>
#include <vector>

#include "board_helper.hpp"
#include "move.hpp"


Engine::Engine(Board& board, BoardHelper& board_helper) 
    : board(board)
    , board_helper(board_helper)
    , move_gen(board, board_helper)
    , game_over_detector(board, move_gen)
{}



Move Engine::iterative_deepening_search_time(int allocated_time_ms) {
    return iterative_deepening_search(NO_CONSTRAINT, allocated_time_ms);
}
Move Engine::iterative_deepening_search_depth(int search_depth) {
    return iterative_deepening_search(search_depth, NO_CONSTRAINT);
}

void Engine::divide(int depth) {
    move_gen.divide(depth);
}

Move Engine::iterative_deepening_search(int search_depth, int allocated_time_ms) {
    nodes = 0;
    time = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    depth = 1;
    while (depth <= search_depth && time < allocated_time_ms) {
        int evaluation_at_depth = search_root(depth, allocated_time_ms - time);
        if (evaluation_at_depth == NO_TIME_LEFT) {
            break;
        }
        score = evaluation_at_depth;
        auto stop_time = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        time = duration == 0 ? 1 : duration;
        show_uci_info();
        depth++;
    }
    return best_move;
}

int Engine::search_root(int depth, int time_left) {
    auto start_time = std::chrono::high_resolution_clock::now();
    int alpha = -200000;
    int beta = -alpha;

    Move best_move_at_depth;
    std::vector<Move> principal_variation;
    std::vector<Move> legal_moves = move_gen.get_legal_moves(false);
    move_ordering(legal_moves, this->depth - depth);
    for (Move& move : legal_moves) {
        std::vector<Move> variation;
        board_helper.make_appropriate(move);

        auto stop_time = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        const int evaluation = -search(depth - 1, -beta, -alpha, time_left - duration, variation);

        board_helper.undo_appropriate();

        if (evaluation == NO_TIME_LEFT) {
            return evaluation;
        }

        if (evaluation > alpha) {
            alpha = evaluation;
            best_move_at_depth = move;
            variation.insert(variation.begin(), move);
            principal_variation = variation;
        }
    }
    best_move = best_move_at_depth;
    pv = principal_variation;
    return alpha;
}

int Engine::search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation) {
    if (time_left <= 0) {
        return NO_TIME_LEFT; 
    }
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<Move> legal_moves = move_gen.get_legal_moves(false);
    move_ordering(legal_moves, this->depth - depth);
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
        std::vector<Move> line;
        board_helper.make_appropriate(move);
        auto stop_time = std::chrono::high_resolution_clock::now();
        int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        const int evaluation = -search(depth - 1, -beta, -alpha, time_left - time_spent, line);
        board_helper.undo_appropriate();

        if (evaluation == NO_TIME_LEFT) {
            return evaluation;
        }

        if (evaluation >= beta) {
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
            line.insert(line.begin(), move);
            principal_variation = line;
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
    nodes++;

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
    std::cout << " depth " << depth;
    std::cout << " score cp " << score;
    std::cout << " nodes " << nodes;
    std::cout << " nps " << nodes * 1000 / time;
    std::cout << " time " << time;
    std::cout << " pv";
    for (Move move : pv) {
        std::cout << " " << move.to_uci_notation();
    }
    std::cout << "\n";
}

void Engine::move_ordering(std::vector<Move>& legal_moves, int current_depth) const {
    if (pv.size() > current_depth) {
        auto pv_move = std::find(legal_moves.begin(), legal_moves.end(), pv.at(current_depth));
        if (pv_move != legal_moves.end()) {
            std::rotate(legal_moves.begin(), pv_move, pv_move + 1);
        }
    }
}
