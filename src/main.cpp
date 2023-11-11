#include <iostream>
#include <string>
#include <thread>

#include "board.hpp"
#include "uci.hpp"


int main() {
    Board board = Board::get_starting_position();
    UCI uci = UCI(board);

    std::string input;
    while (true) {
        std::getline(std::cin, input);

        std::thread thread([&uci, input] {
            uci.process(input);
        });
        thread.detach();
    }

    return 0;
}
