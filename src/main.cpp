#include <iostream>
#include <string>

#include "board.hpp"
#include "uci.hpp"

int main() {
  std::unique_ptr<Board> board = Board::get_starting_position();

  std::string input;
  while (true) {
    std::getline(std::cin, input);
    uci::process(input, board);
  }

  return 0;
}
