#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "game_state.h"
#include "move.h"
#include "game_over_conditions.h"
#include "piece.h"
#include "engine/engine.h"

int main(void) {
    Board board;
    setup_board(&board);
    MoveArray move_history = create_empty_move_history();

    while (true) {
        show_board(&board);

        if (is_checkmate(&board, &move_history)) {
            Color winner = board.player_to_move == WHITE ? BLACK : WHITE;
            printf("%s won!\n", winner == WHITE ? "White" : "Black");
            break;
        } else if (is_draw(&board, &move_history)) {
            printf("It's a draw\n");
            break;
        }

        Move move;
        bool legal_move = false;
        while (!legal_move) {
            char move_uci[6];
            printf("Move: ");
            scanf("%s", move_uci);
            move = uci_notation_to_move(move_uci, &board);

            if (is_legal_move(&move, &board, &move_history)) {
                legal_move = true;
            } else {
                printf("\nThat's not a legal move\n\n");
            }
        }

        make_appropriate_move(&move, &board, &move_history);
    }

    deallocate_game_resources(&board, &move_history);
}
