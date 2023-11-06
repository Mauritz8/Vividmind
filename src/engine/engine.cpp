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
    // keep track of when the search started,
    // so that it can stop if the allocated time runs out
    auto start_time = std::chrono::high_resolution_clock::now();


    // initialize alpha/beta to the value of immediate checkmate 
    // so any legal move will be better
    const int alpha = -CHECKMATE;
    const int beta = -alpha;

    // Create a new SearchInfo object 
    // it contains all the relevant info about the search
    search_info = SearchInfo();

    // search the position at increasing depths
    // until either the given search_depth is reached,
    // or the allocated time has been used up
    while (search_info.depth < search_depth) {
        search_info.depth++;

        // will be updated every time a new best line is found
        std::vector<Move> principal_variation;

        // alpha-beta function
        // evaluate the position at the current depth
        const int evaluation = search(search_info.depth, alpha, beta, allocated_time_ms - search_info.time, principal_variation);

        // if the search has been terminated early 
        // then we can't use the result from the search at this depth 
        // will only happen if the allocated time has been used up
        if (search_info.is_terminated) {
            break;
        }

        // otherwise a valid evaluation was returned, so we can store the result
        auto stop_time = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        search_info.time = duration == 0 ? 1 : duration;
        search_info.score = evaluation;
        search_info.pv = principal_variation;

        show_uci_info();
    }
    // always finish a search by outputting the best move
    const Move& best_move = search_info.pv.at(0);
    std::cout << "bestmove " << best_move.to_uci_notation() << "\n";
}

// alpha-beta evaluation function
int Engine::search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation) {
    // if the time is almost up we have to stop the search
    // but only if a full search has been completed to depth 1
    // otherwise there will be no result from the search
    if (time_left <= MOVE_OVERHEAD && search_info.depth > 1) {
        search_info.is_terminated = true;
        return 0; 
    }

    // keep track of when the search started
    // so that it can calculate how much time it has left
    const auto start_time = std::chrono::high_resolution_clock::now();


    std::vector<Move> pseudo_legal_moves = move_gen.get_pseudo_legal_moves(ALL);


    if (game_over_detector.is_checkmate(pseudo_legal_moves)) {
        // if it is checkmate return large negative value + ply_to_mate
        // to score faster checkmates higher
        const int ply_to_mate = search_info.depth - depth;
        return -CHECKMATE + ply_to_mate;
    }
    if (game_over_detector.is_insufficient_material() || game_over_detector.is_threefold_repetition()) {
        return DRAW;
    }

    // after the search has concluded, 
    // see if there are any winning/losing captures in the position
    // that might change the evaluation of the position
    if (depth == 0) {
        const auto stop_time = std::chrono::high_resolution_clock::now();
        const int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        return search_captures(alpha, beta, time_left - time_spent);
    }


    // try to order the moves by how good they are
    // so that good moves are considered first
    // that way more moves can be pruned away by alpha-beta
    move_ordering(pseudo_legal_moves, search_info.depth - depth);

    const Color player = board.game_state.player_to_move;
    for (const Move& move : pseudo_legal_moves) {

        board.make(move);
        // if the move leaves the king in check, it was not legal
        // so go to the next move
        if (move_gen.is_in_check(player)) {
            board.undo();
            continue;
        }

        const auto stop_time = std::chrono::high_resolution_clock::now();
        const int time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        std::vector<Move> variation;

        // call search function again and decrease the depth
        const int evaluation = -search(depth - 1, -beta, -alpha, time_left - time_spent, variation);

        board.undo();

        // if the search has been terminated return immediately
        // will only happen if the allocated time has been used up
        if (search_info.is_terminated) {
            return 0;
        }

        // if the evaluation is better than beta
        // it means that the opponent has a better option
        // and we don't have to consider this any further
        if (evaluation >= beta) {
            return beta;
        }

        // if the evaluation is higher than alpha
        // then this move is the best so far
        // then we can update alpha 
        // and set the principal variation to the line that gave this evaluation 
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

    // return the best evaluation that was found
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

// the evaluation function is very straight forward
// it only considers material and piece-square tables
int Engine::evaluate() {
    search_info.nodes++;

    // the evaluation from white's point of view
    int evaluation = 0;

    // calculate the material balance from white's point of view
    const int white_material = board.game_state.material[WHITE];
    const int black_material = board.game_state.material[BLACK];
    evaluation += white_material - black_material;

    // calculate the difference between the piece-square table values 
    // also from white's point of view
    const int white_psqt = board.game_state.psqt[WHITE];
    const int black_psqt = board.game_state.psqt[BLACK];
    evaluation += white_psqt - black_psqt;

    // always return the score from the view of the player to move
    // so a positive value always means a good position
    // and a negative value means a bad position
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
