#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"

int main(void) {
    Board board;
    setup_board(&board);

    Color player_to_move = WHITE;
    while (true) {
        show_board(&board);

        if (is_checkmated(player_to_move, &board)) {
            printf("Game over!\n");
            break;
        }

        Move move;
        bool legal_move = false;
        while (!legal_move) {
            char move_uci[4];
            printf("Move: ");
            scanf("%s", move_uci);
            move = uci_notation_to_move(move_uci, &board);

            if (is_legal_move(&move, &board)) {
                legal_move = true;
            } else {
                printf("\nThat's not a legal move\n\n");
            }
        }
        make_move(&move, &board);

        if (player_to_move == WHITE) {
            player_to_move = BLACK;
        } else {
            player_to_move = WHITE;
        }
    }

    deallocate_board(&board);
}
