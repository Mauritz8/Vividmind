#include "engine/engine.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits.h>
#include <optional>
#include <vector>

#include "board_helper.hpp"
#include "move.hpp"
#include "piece.hpp"


Engine::Engine(Board& board, BoardHelper& board_helper) 
    : board(board)
    , board_helper(board_helper)
    , move_gen(board, board_helper)
    , game_over_detector(board, move_gen)
{}



void Engine::iterative_deepening_search_time(int allocated_time_ms) {
    iterative_deepening_search(NO_CONSTRAINT, allocated_time_ms);
}
void Engine::iterative_deepening_search_depth(int search_depth) {
    iterative_deepening_search(search_depth, NO_CONSTRAINT);
}
void Engine::iterative_deepening_search_infinite() {
    iterative_deepening_search(NO_CONSTRAINT, NO_CONSTRAINT);
}

void Engine::divide(int depth) {
    move_gen.divide(depth);
}

int Engine::get_allocated_time() {
    if (board.game_state.player_to_move == WHITE) {
        return search_params.wtime / 20;
    }
    return search_params.btime / 20;
}

void Engine::iterative_deepening_search(int search_depth, int allocated_time_ms) {
    search_result = SearchResult();
    auto start_time = std::chrono::high_resolution_clock::now();
    while (search_result.depth < search_depth && search_result.time < allocated_time_ms) {
        search_result.depth++;
        const int evaluation_at_depth = search_root(search_result.depth, allocated_time_ms - search_result.time);

        if (evaluation_at_depth == NO_TIME_LEFT) {
            break;
        }

        auto stop_time = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        search_result.time = duration == 0 ? 1 : duration;
        show_uci_info();
    }
    std::cout << "bestmove " << search_result.best_move.to_uci_notation() << "\n";
}

int Engine::search_root(int depth, int time_left) {
    auto start_time = std::chrono::high_resolution_clock::now();
    int alpha = ALPHA_INITIAL_VALUE;
    int beta = -alpha;

    Move best_move_at_depth;
    std::vector<Move> principal_variation;
    std::vector<Move> pseudo_legal_moves = move_gen.get_pseudo_legal_moves(false);
    move_ordering(pseudo_legal_moves, search_result.depth - depth);
    const Color player = board.game_state.player_to_move;
    for (Move& move : pseudo_legal_moves) {
        std::vector<Move> variation;
        board_helper.make_appropriate(move);
        if (move_gen.is_in_check(player)) {
            board_helper.undo_appropriate();
            continue;
        }

        auto stop_time = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        const int evaluation = -search(depth - 1, -beta, -alpha, time_left - duration, variation, false);

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
    search_result.score = alpha;
    search_result.best_move = best_move_at_depth;
    search_result.pv = principal_variation;
    return alpha;
}

int Engine::search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation, bool last_was_nullmove) {
    if (time_left <= MOVE_OVERHEAD && search_result.depth > 1) {
        return NO_TIME_LEFT; 
    }
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<Move> legal_moves = move_gen.get_pseudo_legal_moves(false);
    if (game_over_detector.is_checkmate(legal_moves)) {
        const int ply_to_mate = search_result.depth - depth;
        return -CHECKMATE + ply_to_mate;
    }
    if (game_over_detector.is_draw(legal_moves)) {
        return 0;
    }

    if (depth == 0) {
        auto stop_time = std::chrono::high_resolution_clock::now();
        int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        return search_captures(alpha, beta, time_left - time_spent);
    }


    // null move pruning
    const Color player = board.game_state.player_to_move;
    if (depth > 1 && !last_was_nullmove && !move_gen.is_in_check(player)) {
        std::vector<Move> variation;
        board_helper.make_null_move();
        const int shallow_search_depth = depth < 4 ? 0 : depth - 3;
        int evaluation = -search(shallow_search_depth, -beta, -alpha, time_left, variation, true); 
        board_helper.undo_null_move();

        if (evaluation >= beta) {
            return beta;
        }
    }

    move_ordering(legal_moves, search_result.depth - depth);
    for (Move& move : legal_moves) {
        std::vector<Move> variation;
        board_helper.make_appropriate(move);
        if (move_gen.is_in_check(player)) {
            board_helper.undo_appropriate();
            continue;
        }

        auto stop_time = std::chrono::high_resolution_clock::now();
        int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        const int evaluation = -search(depth - 1, -beta, -alpha, time_left - time_spent, variation, false);
        board_helper.undo_appropriate();

        if (evaluation == NO_TIME_LEFT) {
            return evaluation;
        }

        if (evaluation >= beta) {
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
            variation.insert(variation.begin(), move);
            principal_variation = variation;
        }
    }
    // if alpha is still it's initial value
    // no move was possible, which means it is stalemate
    const bool is_stalemate = alpha == ALPHA_INITIAL_VALUE;
    if (is_stalemate) {
        return 0;
    }
    return alpha;
}

int Engine::search_captures(int alpha, int beta, int time_left) {
    if (time_left <= MOVE_OVERHEAD && search_result.depth > 1) {
        return NO_TIME_LEFT; 
    }
    auto start_time = std::chrono::high_resolution_clock::now();

    int evaluation = evaluate();
    if (evaluation >= beta) {
        return beta;
    }
    if (evaluation > alpha) {
        alpha = evaluation;
    }


    const Color player = board.game_state.player_to_move;
    std::vector<Move> captures = move_gen.get_pseudo_legal_moves(true);
    for (Move& capture : captures) {
        board_helper.make_appropriate(capture);
        if (move_gen.is_in_check(player)) {
            board_helper.undo_appropriate();
            continue;
        }
        auto stop_time = std::chrono::high_resolution_clock::now();
        int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        evaluation = -search_captures(-beta, -alpha, time_left - time_spent);
        board_helper.undo_appropriate();

        if (evaluation == NO_TIME_LEFT) {
            return evaluation;
        }

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
    search_result.nodes++;

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
    std::cout << " depth " << search_result.depth;
    if (search_result.score > CHECKMATE_THRESHOLD) {
        std::cout << " score mate " << CHECKMATE - search_result.score;
    } else {
        std::cout << " score cp " << search_result.score;
    }
    std::cout << " nodes " << search_result.nodes;
    std::cout << " nps " << search_result.nodes * 1000 / search_result.time;
    std::cout << " time " << search_result.time;
    std::cout << " pv";
    for (Move move : search_result.pv) {
        std::cout << " " << move.to_uci_notation();
    }
    std::cout << "\n";
    std::cout.flush();
}

void Engine::move_ordering(std::vector<Move>& legal_moves, int current_depth) const {
    if (search_result.pv.size() > current_depth) {
        auto pv_move = std::find(legal_moves.begin(), legal_moves.end(), search_result.pv.at(current_depth));
        if (pv_move != legal_moves.end()) {
            std::rotate(legal_moves.begin(), pv_move, pv_move + 1);
        }
    }
}
