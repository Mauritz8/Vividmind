#include "search.hpp"

#include <cassert>
#include <chrono>
#include <fmt/core.h>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <vector>

#include "board/board.hpp"
#include "engine/move_sort.hpp"
#include "evaluation/evaluation.hpp"
#include "move.hpp"
#include "uci.hpp"

namespace search {
static int time_elapsed(
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time) {
  auto now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
      .count();
}

static bool terminate_search(SearchParams params) {
  bool out_of_time = time_elapsed(params.start_time) > params.allocated_time;
  // depth-1 search must have been completed so we can output a move
  return params.depth >= 2 && (params.stop || out_of_time);
}

static std::optional<std::pair<int, std::forward_list<Move>>>
quiescence(int alpha, int beta, int ply_from_root, int quiescence_plies,
           Board &board, const SearchParams params, SearchInfo &info) {
  if (ply_from_root > info.seldepth) {
    info.seldepth = ply_from_root;
  }

  if (terminate_search(params)) {
    return std::nullopt;
  }

  if (board.is_insufficient_material() || board.is_threefold_repetition() ||
      board.is_draw_by_fifty_move_rule()) {
    return std::make_pair(DRAW, std::forward_list<Move>{});
  }

  std::vector<Move> legal_moves = board.get_legal_moves();
  const bool in_check = board.is_in_check(board.get_player_to_move());
  if (legal_moves.empty()) {
    int eval = in_check ? -CHECKMATE + ply_from_root : DRAW;
    return std::make_pair(eval, std::forward_list<Move>{});
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
  if (!in_check) {
    info.nodes++;
    const int evaluation = evaluate(board);
    if (evaluation >= beta) {
      return std::make_pair(beta, std::forward_list<Move>{});
    }
    if (evaluation > alpha) {
      alpha = evaluation;
    }
  }

  const int QUIESCENCE_CHECKS_MAX_PLY = 5;
  std::vector<Move> moves =
      in_check && quiescence_plies < QUIESCENCE_CHECKS_MAX_PLY
          ? legal_moves
          : board.get_forcing_moves(legal_moves);
  std::unordered_set<Move, Move::HashFunction> killer_moves = {};
  sort_moves(moves, std::nullopt, killer_moves, board);
  std::forward_list<Move> principal_variation = {};
  for (const Move &move : moves) {
    board.make(move);
    auto res = quiescence(-beta, -alpha, ply_from_root + 1,
                          quiescence_plies + 1, board, params, info);
    if (!res.has_value()) {
      return std::nullopt;
    }
    const int evaluation = -res.value().first;
    std::forward_list<Move> variation = res.value().second;
    variation.push_front(move);
    board.undo();

    if (evaluation >= beta) {
      return std::make_pair(beta, variation);
    }
    if (evaluation > alpha) {
      alpha = evaluation;
      principal_variation = variation;
    }
  }
  return std::make_pair(alpha, principal_variation);
}

static std::optional<std::pair<int, std::forward_list<Move>>>
alpha_beta(int depth, int alpha, int beta, int ply_from_root, Board &board,
           const SearchParams params, SearchInfo &info) {
  if (ply_from_root > info.seldepth) {
    info.seldepth = ply_from_root;
  }

  if (terminate_search(params)) {
    return std::nullopt;
  }

  // after the search has concluded,
  // see if there are any winning/losing forcing moves in the position
  // that might change the evaluation of the position
  if (depth == 0) {
    return quiescence(alpha, beta, ply_from_root, 0, board, params, info);
  }

  if (board.is_insufficient_material() || board.is_threefold_repetition() ||
      board.is_draw_by_fifty_move_rule()) {
    return std::make_pair(DRAW, std::forward_list<Move>{});
  }

  std::vector<Move> moves = board.get_legal_moves();
  if (moves.empty()) {
    const int eval = board.is_in_check(board.get_player_to_move())
                         ? -CHECKMATE + ply_from_root
                         : DRAW;
    return std::make_pair(eval, std::forward_list<Move>{});
  }

  std::optional<Move> best_move_prev_depth = std::nullopt;
  if (ply_from_root < std::distance(info.principal_variation.begin(),
                                    info.principal_variation.end())) {
    auto move_it = info.principal_variation.begin();
    std::advance(move_it, ply_from_root);
    best_move_prev_depth = std::make_optional(*move_it);
  }
  sort_moves(moves, best_move_prev_depth, info.killer_moves[ply_from_root],
             board);
  std::forward_list<Move> principal_variation;
  for (const Move &move : moves) {
    board.make(move);
    auto res = alpha_beta(depth - 1, -beta, -alpha, ply_from_root + 1, board,
                          params, info);
    if (!res.has_value()) {
      return std::nullopt;
    }
    const int evaluation = -res.value().first;
    std::forward_list<Move> variation = res.value().second;
    variation.push_front(move);
    board.undo();

    // the move is too good so the opponent will not enter this variation
    if (evaluation >= beta) {
      // because the move was so good, try to refute the opponents other
      // moves with it as well
      info.killer_moves[ply_from_root].insert(move);
      return std::make_pair(beta, variation);
    }

    // the move is the best so far
    if (evaluation > alpha) {
      alpha = evaluation;
      principal_variation = variation;
    }
  }
  return std::make_pair(alpha, principal_variation);
}

void iterative_deepening_search(Board &board, int depth, int allocated_time,
                                std::atomic<bool> &stop) {
  const auto start_time = std::chrono::high_resolution_clock::now();
  SearchInfo info = {
      .seldepth = 0,
      .nodes = 0,
      .principal_variation = {},
      .killer_moves = {},
  };
  stop = false;
  for (int current_depth = 1; current_depth <= depth; current_depth++) {
    SearchParams params = {
        .depth = current_depth,
        .allocated_time = allocated_time,
        .start_time = start_time,
        .stop = stop,
    };
    // initialize alpha and beta to the value of immediate checkmate
    // so any legal move will be considered better
    const auto res = alpha_beta(current_depth, -CHECKMATE, CHECKMATE, 0, board,
                                params, info);
    if (!res.has_value()) {
      break;
    }

    const auto [evaluation, principal_variation] = res.value();
    info.principal_variation = principal_variation;
    SearchSummary search_summary = {.depth = current_depth,
                                    .seldepth = info.seldepth,
                                    .score = evaluation,
                                    .nodes = info.nodes,
                                    .time = time_elapsed(start_time),
                                    .pv = principal_variation};
    fmt::println("{}", uci::show(search_summary));
    std::flush(std::cout);
  }
  assert(!info.principal_variation.empty());
  fmt::println("{}", uci::bestmove(info.principal_variation.front()));
  std::flush(std::cout);
}
} // namespace search
