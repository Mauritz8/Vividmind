#include "engine.hpp"
#include "board/board.hpp"
#include "engine/command.hpp"
#include "engine/search.hpp"
#include "engine/search_defs.hpp"
#include "engine/time_management.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include "perft.hpp"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <unistd.h>

namespace engine {
bool make_move(const char *move_uci, Board &board) {
  const Color player = board.get_player_to_move();
  const std::vector<Move> pseudo_legal_moves =
      board.get_pseudo_legal_moves(ALL);
  for (const Move &move : pseudo_legal_moves) {
    if (move.to_uci_notation() == std::string(move_uci)) {
      board.make(move);
      if (board.is_in_check(player)) {
        board.undo();
        throw std::invalid_argument(fmt::format(
            "Illegal move: {} puts the player in check\n", move_uci));
      }
      return true;
    }
  }
  throw std::invalid_argument(
      fmt::format("Illegal move: {} is not a legal move\n", move_uci));
}

void execute_command(const Command &command, std::atomic<bool> &stop,
                     Board &board) {
  switch (command.type) {
  case UCI: {
    fmt::println("id name {} {}\nid author {}\nuciok\n", NAME, VERSION, AUTHOR);
    break;
  }
  case IsReady: {
    fmt::println("readyok\n");
    break;
  }
  case Invalid: {
    fmt::println("invalid input: {}", command.arg.str);
    free(command.arg.str);
    break;
  }
  case Quit: {
    exit(0);
    break;
  }
  case UpdateBoard: {
    Position position = command.arg.position;
    try {
      board = fen::get_position(position.fen);
      for (size_t i = 0; i < position.moves_size; i++) {
        make_move(position.moves[i], board);
      }
    } catch (const std::invalid_argument &e) {
      fmt::println(e.what());
    }
    free(position.fen);
    for (size_t i = 0; i < position.moves_size; i++) {
      free(position.moves[i]);
    }
    free(position.moves);
    break;
  }
  case GoPerft: {
    divide(board, command.arg.integer);
    break;
  }
  case GoInfinite: {
    SearchParams params = SearchParams();
    params.search_mode = SearchMode::INFINITE;
    Search search = Search(board, params, stop);
    search.iterative_deepening_search();
    break;
  }
  case GoDepth: {
    SearchParams params = SearchParams();
    params.search_mode = SearchMode::DEPTH;
    params.depth = command.arg.integer;
    Search search = Search(board, params, stop);
    search.iterative_deepening_search();
    break;
  }
  case GoGameTime: {
    SearchParams params = SearchParams();
    params.search_mode = SearchMode::MOVE_TIME;
    params.allocated_time = calc_allocated_time(board.get_player_to_move(),
                                                command.arg.game_time.wtime,
                                                command.arg.game_time.btime);
    Search search = Search(board, params, stop);
    search.iterative_deepening_search();
    break;
  }
  case GoMoveTime: {
    SearchParams params = SearchParams();
    params.search_mode = SearchMode::MOVE_TIME;
    // to ensure a move is returned before the allocated time runs out
    int move_overhead = 50;
    params.allocated_time = command.arg.integer - move_overhead;
    Search search = Search(board, params, stop);
    search.iterative_deepening_search();
    break;
  }
  }
  std::flush(std::cout);
}
}; // namespace engine
