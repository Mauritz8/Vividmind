#include <iostream>
#include <string>

#include "board.hpp"
#include "engine/uci.hpp"


int main() {
    Board board = Board::get_starting_position();
    UCI uci = UCI(board);

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        uci.process_command(input);
    }

    return 0;
}
