#include "engine.hpp"
#include "search/command.hpp"
#include "search/search.hpp"
#include "search/search_defs.hpp"
#include <unistd.h>

Engine::Engine() : board(Board::get_starting_position()) {}

SearchParams Engine::get_search_params(Command &command) const {
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
    // todo
    break;
  case GoPerft:
    // todo
    break;
  }
  return params;
}

void Engine::run(int read_descriptor) {
  Command command;
  while (true) {
    read(read_descriptor, &command, sizeof(command));

    SearchParams params = get_search_params(command);
    Search search = Search(board, params);
    search.iterative_deepening_search();
  }
}
