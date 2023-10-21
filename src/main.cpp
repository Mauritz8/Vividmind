#include <iostream>
#include <string>
#include <vector>

#include "board.h"
#include "engine/uci.h"
#include "game_state.h"


int main(void) {
    Board board = Board::get_starting_position();

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        process_uci_command(input, board);
    }

    return 0;
}
