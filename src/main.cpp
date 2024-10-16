#include <iostream>
#include <string>

#include "mailbox_board/mailbox_board.hpp"
#include "uci.hpp"

int main() {
  MailboxBoard board = MailboxBoard::get_starting_position();

  std::string input;
  while (true) {
    std::getline(std::cin, input);
    uci::process(input, board);
  }

  return 0;
}
