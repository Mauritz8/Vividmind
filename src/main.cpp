#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "board.h"
#include "engine/uci.h"
#include "game_state.h"

int main(void) {
    Board board;
    board.setup_board();
    std::vector<Move> move_history;

    const int UCI_COMMAND_CHARACTER_LIMIT = 2048;
    char input[UCI_COMMAND_CHARACTER_LIMIT];
    while (true) {
        fgets(input, UCI_COMMAND_CHARACTER_LIMIT, stdin);
        input[strcspn(input, "\n")] = '\0';
        process_uci_command(input, board, move_history);
    }

    return 0;
}
