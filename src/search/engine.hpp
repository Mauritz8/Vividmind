#pragma once

#include "board.hpp"
#include "search/command.hpp"
#include "search/search_defs.hpp"
#include <memory>


class Engine {
public:
  Engine();
  void run(int read_descriptor);

private:
  std::unique_ptr<Board> board;

  SearchParams get_search_params(Command &command) const;
};
