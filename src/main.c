#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "engine/uci.h"
#include "game_state.h"

int main(void) {
    Board board;
    setup_board(&board);
    MoveArray move_history = create_empty_move_history();

    char input[200];
    while (true) {
        fgets(input, 200, stdin);
        input[strcspn(input, "\n")] = '\0';
        process_uci_command(input, &board, &move_history);
    }

    deallocate_game_resources(&board, &move_history);
    return 0;
}
