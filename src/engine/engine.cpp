#include "engine.hpp"
#include "engine/command.hpp"
#include "engine/search.hpp"
#include "engine/search_defs.hpp"
#include "engine/time_management.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include "perft.hpp"
#include <memory>
#include <stdexcept>
#include <unistd.h>

namespace engine {
void run(int read_descriptor, std::atomic<bool> &stop) {
  std::unique_ptr<Board> board = Board::get_starting_position();

  Command command;
  while (true) {
    read(read_descriptor, &command, sizeof(command));

    switch (command.type) {
    case UpdateBoard: {
      try {
        board = fen::get_position(command.arg.str);
      } catch (const std::invalid_argument &e) {
        fmt::println(e.what());
      }
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
      params.allocated_time = calc_allocated_time(
        board->get_player_to_move(),
        command.arg.game_time.wtime,
        command.arg.game_time.btime
      );
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
  }
}
}; // namespace engine
