#include "search.hpp"

#include <algorithm>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <ostream>
#include <vector>

#include "board/board.hpp"
#include "defs.hpp"
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
  };
  stop = false;
  std::optional<Move> best_move = std::nullopt;
  while (info.depth < params.depth && !info.is_terminated) {
    info.depth++;

    // initialize alpha and beta to the value of immediate checkmate
    // so any legal move will be considered better
    const int alpha = -CHECKMATE;
    const int beta = CHECKMATE;
    std::vector<Move> principal_variation;
    const int evaluation =
        alpha_beta(info.depth, alpha, beta, principal_variation, best_move);

    if (!info.is_terminated) {
      SearchSummary search_summary = {.depth = info.depth,
                                      .seldepth = info.seldepth,
                                      .score = evaluation,
                                      .nodes = info.nodes,
                                      .time = time_elapsed(info.start_time),
                                      .pv = principal_variation};
      assert(search_summary.pv.size() > 0);
      best_move = search_summary.pv.at(0);

      fmt::println("{}", uci::show(search_summary));
      std::flush(std::cout);
    }
  }
  assert(best_move.has_value());
  fmt::println("{}", uci::bestmove(best_move.value()));
  std::flush(std::cout);
}

int Search::alpha_beta(int depth, int alpha, int beta,
                       std::vector<Move> &principal_variation,
                       const std::optional<Move> &best_move_prev_depth) {
  const Color player = board.get_player_to_move();

  if (is_terminate()) {
    info.is_terminated = true;
    return 0;
  }

  // look one move further if the player is in check because the opponent
  // could have a strong next move after we move out of check
  const bool is_in_check = board.is_in_check(player);
  if (is_in_check) {
    depth++;
  }

  // after the search has concluded,
  // see if there are any winning/losing captures in the position
  // that might change the evaluation of the position
  if (depth == 0) {
    return quiescence(alpha, beta, principal_variation);
  }

  std::vector<Move> pseudo_legal_moves = board.get_pseudo_legal_moves();
  sort_moves(pseudo_legal_moves);

  if (best_move_prev_depth.has_value()) {
    auto it = std::find(pseudo_legal_moves.begin(), pseudo_legal_moves.end(),
                        best_move_prev_depth);
    if (it != pseudo_legal_moves.end()) {
      std::rotate(pseudo_legal_moves.begin(), it, it + 1);
    }
  }

  int legal_moves_found = 0;
  for (const Move &move : pseudo_legal_moves) {

    board.make(move);
    if (board.is_in_check(player)) {
      board.undo();
      continue;
    }
    legal_moves_found++;

    info.ply_from_root++;
    if (info.ply_from_root > info.seldepth) {
      info.seldepth = info.ply_from_root;
    }

    std::vector<Move> variation;
    int evaluation = board.is_draw() ? DRAW
                                     : -alpha_beta(depth - 1, -beta, -alpha,
                                                   variation, std::nullopt);

    board.undo();
    info.ply_from_root--;

    // the move is guaranteed to be worse than a previous move we could play
    // so we don't have to consider this variation any further
    if (evaluation >= beta) {
      return beta;
    }

    // the move is the best so far
    if (evaluation > alpha) {
      alpha = evaluation;
      variation.insert(variation.begin(), move);
      principal_variation = variation;
    }
  }
  if (legal_moves_found > 0) {
    return alpha;
  }
  return is_in_check ? -CHECKMATE + info.ply_from_root : DRAW;
}

int Search::quiescence(int alpha, int beta,
                       std::vector<Move> &principal_variation) {
  if (is_terminate()) {
    info.is_terminated = true;
    return 0;
  }

  info.nodes++;
  int evaluation = evaluate(board);
  if (evaluation >= beta) {
    return beta;
  }
  if (evaluation > alpha) {
    alpha = evaluation;
  }

  const Color player = board.get_player_to_move();
  std::vector<Move> captures = board.get_pseudo_legal_moves(true);
  sort_moves(captures);
  for (const Move &capture : captures) {
    board.make(capture);
    if (board.is_in_check(player)) {
      board.undo();
      continue;
    }

    info.ply_from_root++;
    if (info.ply_from_root > info.seldepth) {
      info.seldepth = info.ply_from_root;
    }

    std::vector<Move> variation;
    evaluation = -quiescence(-beta, -alpha, variation);
    board.undo();
    info.ply_from_root--;

    if (evaluation >= beta) {
      return beta;
    }
    if (evaluation > alpha) {
      alpha = evaluation;
      variation.insert(variation.begin(), capture);
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

void Search::sort_moves(std::vector<Move> &moves) {
  auto sort_mvv_lva = [&](Move i, Move j) {
    return get_move_score(i) > get_move_score(j);
  };
  std::sort(moves.begin(), moves.end(), sort_mvv_lva);
}

int Search::get_move_score(const Move &move) {
  const std::optional<PieceType> start_piece = board.get_piece_type(move.start);
  const std::optional<PieceType> end_piece = board.get_piece_type(move.end);

  // score non-capture moves lower than captures
  if (!end_piece) {
    return -QUEEN_VALUE;
  }
  return PIECE_VALUES.at(end_piece.value()) -
         PIECE_VALUES.at(start_piece.value());
}
