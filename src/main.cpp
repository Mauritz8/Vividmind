#include <iostream>
#include <string>

#include "board/board.hpp"
#include "uci.hpp"

int main() {
  Board board = Board::get_starting_position();

  std::string input;
  while (true) {
    std::getline(std::cin, input);
    uci::process(input, board);
  }

  return 0;
}
