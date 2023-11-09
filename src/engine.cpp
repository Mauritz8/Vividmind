#include "engine.hpp"

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
#include "uci.hpp"


Engine::Engine(Board& board, MoveGenerator& move_gen) 
    : board(board)
    , move_gen(move_gen)
{}

void Engine::iterative_deepening_search() {
    // initialize alpha/beta to the value of immediate checkmate 
    // so any legal move will be considered better
    const int alpha = -CHECKMATE;
    const int beta = -alpha;

    // Create a new SearchInfo object 
    // it contains all the relevant info about the search
    search_info = SearchInfo();

    // keep track of when the search started,
    // so that it can stop if the allocated time runs out
    search_info.start_time = std::chrono::high_resolution_clock::now();


    // will be updated whenever a new best move is found
    Move best_move;

    // search the position at increasing depths
    // until either the final depth is reached,
    // or it is terminated
    while (search_info.depth < search_params.depth && !search_info.is_terminated) {
        search_info.depth++;

        // will be updated every time a new best line is found
        std::vector<Move> principal_variation;

        // alpha-beta function
        // evaluate the position at the current depth
        const int evaluation = search(search_info.depth, alpha, beta, principal_variation);

        // if the search has not been terminated
        // then we can use the result from the search at this depth 
        if (!search_info.is_terminated) {
            SearchSummary search_summary = {
                .depth = search_info.depth,
                .score = evaluation,
                .nodes = search_info.nodes,
                .time = search_info.time_elapsed(),
                .pv = principal_variation
            };
            best_move = search_summary.pv.at(0);

            UCI::show(search_summary);
        }

    }
    // always finish a search by outputting the best move
    std::cout << "bestmove " << best_move.to_uci_notation() << "\n";
}

int Engine::get_allocated_time() {
    if (board.game_state.player_to_move == WHITE) {
        return search_params.game_time.wtime / 20;
    }
    return search_params.game_time.btime / 20;
}

// alpha-beta evaluation function
int Engine::search(int depth, int alpha, int beta, std::vector<Move>& principal_variation) {
    // check if the search should terminate
    check_termination();

    // if the search has been terminated, then return immediately
    // it will only happen if the allocated time has been used up
    if (search_info.is_terminated) {
        return 0;
    }

    if (board.is_insufficient_material() || 
        board.is_threefold_repetition()  ||
        board.is_draw_by_fifty_move_rule()) 
    {
        return DRAW;
    }

    // after the search has concluded, 
    // see if there are any winning/losing captures in the position
    // that might change the evaluation of the position
    if (depth == 0) {
        return search_captures(alpha, beta);
    }

    const Color player = board.game_state.player_to_move;
    std::vector<Move> pseudo_legal_moves = move_gen.get_pseudo_legal_moves(ALL);
    int legal_moves_found = 0;
    for (const Move& move : pseudo_legal_moves) {

        board.make(move);
        // if the move leaves the king in check, it was not legal
        // so go to the next move
        if (move_gen.is_in_check(player)) {
            board.undo();
            continue;
        }

        // if the move didn't leave the king in check, it's a legal move
        legal_moves_found++;

        std::vector<Move> variation;

        // call search function again and decrease the depth
        const int evaluation = -search(depth - 1, -beta, -alpha, variation);

        board.undo();

        // if the evaluation is higher than beta
        // it means that the this move is guaranteed to be worse than a previous move we could play
        // so we don't have to consider this variation any further
        if (evaluation >= beta) {
            return beta;
        }

        // if the evaluation is higher than alpha
        // then this move is the best so far
        if (evaluation > alpha) {

            // then we can update alpha accordingly
            alpha = evaluation;

            // and set the principal variation to the line that gave this evaluation 
            variation.insert(variation.begin(), move);
            principal_variation = variation;
        }
    }

    // if there are no legal moves in the position, 
    // it means that it is either checkmate or stalemate
    if (legal_moves_found == 0) {

        // if there were no legal moves and the player is in check
        // it means that it must be checkmate
        if (move_gen.is_in_check(player)) {
            // if it is checkmate return large negative value + ply_to_mate
            // to score faster checkmates higher
            const int ply_to_mate = search_info.depth - depth;
            return -CHECKMATE + ply_to_mate;
        }

        // otherwise it is stalemate
        return DRAW;
    }

    // return the best evaluation that was found
    return alpha;
}

int Engine::search_captures(int alpha, int beta) {
    check_termination();
    if (search_info.is_terminated) {
        return 0;
    }

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
        evaluation = -search_captures(-beta, -alpha);
        board.undo();

        if (evaluation >= beta) {
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
        }
    }

    return alpha;
}

void Engine::check_termination() {
    switch (search_params.search_mode) {
        case DEPTH:
            if (search_info.depth > search_params.depth && search_info.depth > 1) {
                search_info.is_terminated = true;
            }
            break;
        case MOVE_TIME:
            if (search_info.time_elapsed() > search_params.allocated_time) {
                search_info.is_terminated = true;
            }
            break;
        case INFINITE: break;
    }
}

SearchParams::SearchParams() {
    depth = MAX_PLY,
    allocated_time = 0,
    game_time = GameTime{
        .wtime = 0,
        .btime = 0,
        .winc = 0,
        .binc = 0,
        .moves_to_go = 0
    },
    search_mode = INFINITE;
}

int SearchInfo::time_elapsed() const {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
}
