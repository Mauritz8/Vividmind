#include "search.hpp"

#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <ostream>
#include <vector>

#include "board/board.hpp"
#include "engine/move_sort.hpp"
#include "evaluation/evaluation.hpp"
#include "move.hpp"
#include "uci.hpp"

Search::Search(Board &board, SearchParams &params, std::atomic<bool> &stop)
    : board(board), params(params), stop(stop) {}

static int time_elapsed(
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time) {
  auto now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
      .count();
}

void Search::iterative_deepening_search() {
  info = {
      .start_time = std::chrono::high_resolution_clock::now(),
      .depth = 0,
      .ply_from_root = 0,
      .seldepth = 0,
      .nodes = 0,
      .is_terminated = false,
      .best_move = std::nullopt,
      .killer_moves = {},
      .quiescence_plies = 0,
  };
  stop = false;
  while (info.depth < params.depth && !info.is_terminated) {
    info.depth++;

    // initialize alpha and beta to the value of immediate checkmate
    // so any legal move will be considered better
    const int alpha = -CHECKMATE;
    const int beta = CHECKMATE;
    std::vector<Move> principal_variation;
    const int evaluation =
        alpha_beta(info.depth, alpha, beta, principal_variation);

    if (!info.is_terminated) {
      SearchSummary search_summary = {.depth = info.depth,
                                      .seldepth = info.seldepth,
                                      .score = evaluation,
                                      .nodes = info.nodes,
                                      .time = time_elapsed(info.start_time),
                                      .pv = principal_variation};
      assert(search_summary.pv.size() > 0);
      info.best_move = search_summary.pv.at(0);

      fmt::println("{}", uci::show(search_summary));
      std::flush(std::cout);
    }
  }
  assert(info.best_move.has_value());
  fmt::println("{}", uci::bestmove(info.best_move.value()));
  std::flush(std::cout);
}

int Search::alpha_beta(int depth, int alpha, int beta,
                       std::vector<Move> &principal_variation) {
  if (is_terminate()) {
    info.is_terminated = true;
    return 0;
  }

  // look one move further if the player is in check because the opponent
  // could have a strong next move after we move out of check
  const bool is_in_check = board.is_in_check(board.get_player_to_move());
  if (is_in_check) {
    depth++;
  }

  // after the search has concluded,
  // see if there are any winning/losing forcing moves in the position
  // that might change the evaluation of the position
  if (depth == 0) {
    return quiescence(alpha, beta, principal_variation);
  }

  if (board.is_insufficient_material() || board.is_threefold_repetition() ||
      board.is_draw_by_fifty_move_rule()) {
    return DRAW;
  }

  std::vector<Move> moves = board.get_legal_moves();
  if (moves.empty()) {
    return is_in_check ? -CHECKMATE + info.ply_from_root : DRAW;
  }

  sort_moves(moves, info.best_move, info.killer_moves[info.ply_from_root],
             board);
  for (const Move &move : moves) {
    board.make(move);

    info.ply_from_root++;
    if (info.ply_from_root > info.seldepth) {
      info.seldepth = info.ply_from_root;
    }

    std::vector<Move> variation;
    int evaluation = -alpha_beta(depth - 1, -beta, -alpha, variation);

    board.undo();
    info.ply_from_root--;

    // the move is too good so the opponent will not enter this variation
    if (evaluation >= beta) {
      // because the move was so good, try to refute the opponents other
      // moves with it as well
      info.killer_moves[info.ply_from_root].insert(move);
      return beta;
    }

    // the move is the best so far
    if (evaluation > alpha) {
      alpha = evaluation;
      variation.insert(variation.begin(), move);
      principal_variation = variation;
    }
  }
  return alpha;
}

int Search::quiescence(int alpha, int beta,
                       std::vector<Move> &principal_variation) {
  if (is_terminate()) {
    info.is_terminated = true;
    return 0;
  }
  info.nodes++;

  if (board.is_insufficient_material() || board.is_threefold_repetition() ||
      board.is_draw_by_fifty_move_rule()) {
    return DRAW;
  }

  std::vector<Move> legal_moves = board.get_legal_moves();
  const bool in_check = board.is_in_check(board.get_player_to_move());
  if (legal_moves.empty()) {
    return in_check ? -CHECKMATE + info.ply_from_root : DRAW;
  }

  // If you are not in check, you can evaluate the current board because you
  // can always choose to not make any of the forcing moves. However, if you
  // are in check, you can't because the opponent could force a new board state.
  //
  // You have to limit this to a certain depth because in many situations
  // a piece can continually check the king and not taking the piece is
  // often a valid move.
  //
  // This position is a good example:
  // rnb1kbnr/pppp1ppp/8/4p3/4PP1q/6P1/PPPP3P/RNBQKBNR b KQkq - 0 3
  //
  // Quiescence would have to look at sequences like:
  // Qxg3+ Ke2 Qf2+ Kd3 Qf3 Kc4 Qe2
  // and all the possible captures after each move even though
  // the queen can simply be captured.
  int QUIESCENCE_CHECKS_MAX_PLY = 5;
  bool extend_search =
      in_check && info.quiescence_plies < QUIESCENCE_CHECKS_MAX_PLY;
  if (!extend_search) {
    const int evaluation = evaluate(board);
    if (evaluation >= beta) {
      return beta;
    }
    if (evaluation > alpha) {
      alpha = evaluation;
    }
  }

  std::vector<Move> moves =
      extend_search ? legal_moves : board.get_forcing_moves(legal_moves);
  for (const Move &move : moves) {
    board.make(move);

    info.ply_from_root++;
    info.quiescence_plies++;
    if (info.ply_from_root > info.seldepth) {
      info.seldepth = info.ply_from_root;
    }

    std::vector<Move> variation;
    const int evaluation = -quiescence(-beta, -alpha, variation);
    board.undo();
    info.ply_from_root--;
    info.quiescence_plies--;

    if (evaluation >= beta) {
      return beta;
    }
    if (evaluation > alpha) {
      alpha = evaluation;
      variation.insert(variation.begin(), move);
      principal_variation = variation;
    }
  }
  return alpha;
}

bool Search::is_terminate() {
  // search must complete to depth 1 at least so we can output a move
  if (info.depth < 2)
    return false;

  if (stop) {
    return true;
  }

  switch (params.search_mode) {
  case DEPTH:
    if (info.depth > params.depth) {
      return true;
    }
    break;
  case MOVE_TIME:
    if (time_elapsed(info.start_time) > params.allocated_time) {
      return true;
    }
    break;
  case INFINITE:
    return false;
  }
  return false;
}
