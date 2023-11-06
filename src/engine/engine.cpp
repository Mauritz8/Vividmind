#include "engine/engine.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits.h>
#include <optional>
#include <vector>

#include "move.hpp"
#include "move_generator.hpp"
#include "piece.hpp"


Engine::Engine(Board& board) 
    : board(board)
    , move_gen(board)
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
    auto start_time = std::chrono::high_resolution_clock::now();


    int alpha = ALPHA_INITIAL_VALUE;
    int beta = -alpha;

    search_info = SearchInfo();
    while (search_info.depth < search_depth) {
        search_info.depth++;

        std::vector<Move> principal_variation;

        const int evaluation = search(search_info.depth, alpha, beta, allocated_time_ms - search_info.time, principal_variation, false);

        if (search_info.is_terminated) {
            break;
        }

        auto stop_time = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        search_info.time = duration == 0 ? 1 : duration;
        search_info.score = evaluation;
        search_info.pv = principal_variation;

        show_uci_info();
    }
    const Move& best_move = search_info.pv.at(0);
    std::cout << "bestmove " << best_move.to_uci_notation() << "\n";
}

int Engine::search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation, bool last_was_nullmove) {
    if (time_left <= MOVE_OVERHEAD && search_info.depth > 1) {
        search_info.is_terminated = true;
        return 0; 
    }
    const auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<Move> pseudo_legal_moves = move_gen.get_pseudo_legal_moves(ALL);
    if (game_over_detector.is_checkmate(pseudo_legal_moves)) {
        const int ply_to_mate = search_info.depth - depth;
        return -CHECKMATE + ply_to_mate;
    }
    if (game_over_detector.is_insufficient_material() || game_over_detector.is_threefold_repetition()) {
        return DRAW;
    }

    if (depth == 0) {
        const auto stop_time = std::chrono::high_resolution_clock::now();
        const int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        return search_captures(alpha, beta, time_left - time_spent);
    }


    // null move pruning
    const Color player = board.game_state.player_to_move;
    if (depth > 1 && !last_was_nullmove && !move_gen.is_in_check(player)) {
        std::vector<Move> variation;
        board.make_null_move();
        const int shallow_search_depth = depth < 4 ? 0 : depth - 3;
        const int evaluation = -search(shallow_search_depth, -beta, -alpha, time_left, variation, true); 
        board.undo_null_move();

        if (search_info.is_terminated) {
            return 0;
        }

        if (evaluation >= beta) {
            return beta;
        }
    }

    move_ordering(pseudo_legal_moves, search_info.depth - depth);
    for (const Move& move : pseudo_legal_moves) {
        std::vector<Move> variation;
        board.make(move);
        if (move_gen.is_in_check(player)) {
            board.undo();
            continue;
        }

        const auto stop_time = std::chrono::high_resolution_clock::now();
        const int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        const int evaluation = -search(depth - 1, -beta, -alpha, time_left - time_spent, variation, false);
        board.undo();

        if (search_info.is_terminated) {
            return 0;
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
        return DRAW;
    }
    return alpha;
}

int Engine::search_captures(int alpha, int beta, int time_left) {
    if (time_left <= MOVE_OVERHEAD && search_info.depth > 1) {
        search_info.is_terminated = true;
        return 0; 
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
    std::vector<Move> captures = move_gen.get_pseudo_legal_moves(CAPTURES);
    for (const Move& capture : captures) {
        board.make(capture);
        if (move_gen.is_in_check(player)) {
            board.undo();
            continue;
        }
        auto stop_time = std::chrono::high_resolution_clock::now();
        int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        evaluation = -search_captures(-beta, -alpha, time_left - time_spent);
        board.undo();

        if (search_info.is_terminated) {
            return 0;
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
    search_info.nodes++;

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
    std::cout << " depth " << search_info.depth;
    if (std::abs(search_info.score) > CHECKMATE_THRESHOLD) {
        const int ply = CHECKMATE - std::abs(search_info.score);
        const int mate_in_x = std::ceil(ply / 2.0);
        const int sign = search_info.score > 0 ? 1 : -1;
        std::cout << " score mate " << sign * mate_in_x;
    } else {
        std::cout << " score cp " << search_info.score;
    }
    std::cout << " nodes " << search_info.nodes;
    std::cout << " nps " << search_info.nodes * 1000 / search_info.time;
    std::cout << " time " << search_info.time;
    std::cout << " pv";
    for (const Move& move : search_info.pv) {
        std::cout << " " << move.to_uci_notation();
    }
    std::cout << "\n";
    std::cout.flush();
}

void Engine::move_ordering(std::vector<Move>& legal_moves, int current_depth) const {
    if (search_info.pv.size() > current_depth) {
        auto pv_move = std::find(legal_moves.begin(), legal_moves.end(), search_info.pv.at(current_depth));
        if (pv_move != legal_moves.end()) {
            std::rotate(legal_moves.begin(), pv_move, pv_move + 1);
        }
    }
}
