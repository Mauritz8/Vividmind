#include "engine.hpp"
#include "engine/command.hpp"
#include "engine/search.hpp"
#include "engine/search_defs.hpp"
#include "engine/time_management.hpp"
#include "perft.hpp"
#include <memory>
#include <thread>
#include <unistd.h>

namespace engine {
SearchParams get_search_params(Command &command, Color player_to_move) {
  SearchParams params = SearchParams();
  switch (command.type) {
  case GoInfinite:
    params.search_mode = SearchMode::INFINITE;
    break;
  case GoDepth:
    params.search_mode = SearchMode::DEPTH;
    params.depth = command.arg.integer;
    break;
  case GoMoveTime:
    params.search_mode = SearchMode::MOVE_TIME;
    params.allocated_time = command.arg.integer;
    break;
  case GoGameTime:
    params.search_mode = SearchMode::MOVE_TIME;
    params.allocated_time =
        calc_allocated_time(player_to_move, command.arg.game_time.wtime,
                            command.arg.game_time.btime);
    break;
  }
  return params;
}

void loop(int read_descriptor, int write_descriptor, bool &stop, std::unique_ptr<Board> &board) {
  Command command;
  while (true) {
    read(read_descriptor, &command, sizeof(command));

    switch (command.type) {
    case Stop:
      stop = true;
      break;
    case GoPerft:
      divide(board, command.arg.integer);
      break;
    case GoInfinite:
    case GoDepth:
    case GoGameTime:
    case GoMoveTime:
      SearchParams params =
          get_search_params(command, board->get_player_to_move());
      write(write_descriptor, &params, sizeof(params));
      break;
    }
  }
}

void run_search(int read_descriptor, bool &stop, std::unique_ptr<Board> &board) {
  SearchParams params;
  while (true) {
    read(read_descriptor, &params, sizeof(SearchParams));
    Search search = Search(board, params, stop);
    search.iterative_deepening_search();
  }
}

void run(int read_descriptor) {
  std::unique_ptr<Board> board = Board::get_starting_position();
  bool stop = false;

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    exit(1);
  }

  std::thread t1(loop, read_descriptor, pipefd[1], std::ref(stop), std::ref(board));
  std::thread t2(run_search, pipefd[0], std::ref(stop), std::ref(board));

  t1.join();
  t2.join();
}

}; // namespace engine
