#include "search.hpp"

#include <algorithm>
#include <cassert>
#include <fmt/core.h>
#include <optional>
#include <stack>
#include <vector>

#include "defs.hpp"
#include "evaluation/evaluation.hpp"
#include "move.hpp"
#include "engine/search_defs.hpp"
#include "uci.hpp"

Search::Search(std::unique_ptr<Board> &board, SearchParams &params)
    : board(board), params(params) {}

void Search::iterative_deepening_search() {
  // initialize alpha/beta to the value of immediate checkmate
  // so any legal move will be considered better
  const int alpha = -CHECKMATE;
  const int beta = -alpha;

  // Create a new SearchInfo object
  // it contains all the relevant info about the search
  info = SearchInfo();

  // will be updated whenever a new best move is found
  std::stack<Move> best_moves;

  // search the position at increasing depths
  // until either the final depth is reached,
  // or it is terminated
  while (info.depth < params.depth && !info.is_terminated) {
    info.depth++;

    // will be updated every time a new best line is found
    std::vector<Move> principal_variation;

    // alpha-beta function
    // evaluate the position at the current depth
    const int evaluation =
        alpha_beta(info.depth, alpha, beta, principal_variation);

    // if the search has not been terminated
    // then we can use the result from the search at this depth
    if (!info.is_terminated) {
      SearchSummary search_summary = {.depth = info.depth,
                                      .seldepth = info.seldepth,
                                      .score = evaluation,
                                      .nodes = info.nodes,
                                      .time = info.time_elapsed(),
                                      .pv = principal_variation};
      assert(search_summary.pv.size() > 0);
      best_moves.push(search_summary.pv.at(0));

      fmt::println(uci::show(search_summary));
    }
  }
  // always finish a search by outputting the best move
  fmt::println(uci::bestmove(best_moves.top()));
}

int Search::alpha_beta(int depth, int alpha, int beta,
                       std::vector<Move> &principal_variation) {
  const Color player = board->get_player_to_move();

  // if the search has been terminated, then return immediately
  if (is_terminate()) {
    info.is_terminated = true;
    return 0;
  }

  // if player is in check, it's a good idea to look one move further
  // because there could be tactics available
  // after the opponent moves out of the check
  const bool is_in_check = board->is_in_check(player);
  if (is_in_check) {
    depth++;
  }

  // after the search has concluded,
  // see if there are any winning/losing captures in the position
  // that might change the evaluation of the position
  if (depth == 0) {
    return quiescence(alpha, beta, principal_variation);
  }

  std::vector<Move> pseudo_legal_moves = board->get_pseudo_legal_moves(ALL);
  sort_moves(pseudo_legal_moves);

  int legal_moves_found = 0;
  for (const Move &move : pseudo_legal_moves) {

    board->make(move);
    // if the move leaves the king in check, it was not legal
    // so go to the next move
    if (board->is_in_check(player)) {
      board->undo();
      continue;
    }
    info.ply_from_root++;
    if (info.ply_from_root > info.seldepth) {
      info.seldepth = info.ply_from_root;
    }

    // if the move didn't leave the king in check, it's a legal move
    legal_moves_found++;

    std::vector<Move> variation;

    // assume the position is a draw
    int evaluation = DRAW;

    // if it's not a draw we must search further
    if (!board->is_draw()) {
      // call search function again and decrease the depth
      evaluation = -alpha_beta(depth - 1, -beta, -alpha, variation);
    }

    board->undo();
    info.ply_from_root--;

    // if the evaluation is higher than beta
    // it means that the this move is guaranteed to be worse than a previous
    // move we could play so we don't have to consider this variation any
    // further
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
    if (is_in_check) {
      // score faster checkmates higher
      return -CHECKMATE + info.ply_from_root;
    }

    // otherwise it is stalemate
    return DRAW;
  }

  // return the best evaluation that was found
  return alpha;
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

  const Color player = board->get_player_to_move();
  std::vector<Move> captures = board->get_pseudo_legal_moves(TACTICAL);
  sort_moves(captures);
  for (const Move &capture : captures) {
    board->make(capture);
    if (board->is_in_check(player)) {
      board->undo();
      continue;
    }

    info.ply_from_root++;
    if (info.ply_from_root > info.seldepth) {
      info.seldepth = info.ply_from_root;
    }

    std::vector<Move> variation;
    evaluation = -quiescence(-beta, -alpha, variation);
    board->undo();
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
  // don't terminate if search hasn't completed to depth 1 at least
  // because then we haven't found a best move yet
  if (info.depth < 2)
    return false;

  switch (params.search_mode) {
  case DEPTH:
    if (info.depth > params.depth) {
      return true;
    }
    break;
  case MOVE_TIME:
    if (info.time_elapsed() > params.allocated_time) {
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
  const std::optional<PieceType> start_piece =
      board->get_piece_type(move.start);
  const std::optional<PieceType> end_piece = board->get_piece_type(move.end);

  // score non-capture moves lower than captures
  if (!end_piece) {
    return -QUEEN_VALUE;
  }
  return get_piece_value(end_piece.value()) -
         get_piece_value(start_piece.value());
}
