#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "game_over_conditions.h"
#include "piece.h"

int main(void) {
    Board board;
    setup_board(&board);

    MoveArray move_history;
    int move_history_capacity = 16;
    move_history.moves = malloc(move_history_capacity * sizeof(Move));
    move_history.length = 0;

    Color player_to_move = WHITE;
    while (true) {
        show_board(&board);

        if (is_checkmated(player_to_move, &board, &move_history)) {
            Color winner = player_to_move == WHITE ? BLACK : WHITE;
            printf("%s won!\n", winner == WHITE ? "White" : "Black");
            break;
        } else if (is_draw(player_to_move, &board, &move_history)) {
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
        if (is_valid_castling_move(&move, &move_history, &board)) {
            make_castling_move(&move, &board);
        } else if (is_valid_en_passant_move(&move, &board, &move_history)) {
            make_en_passant_move(&move, &board);
        } else if (is_promotion(&move, &board)) {
            make_promotion_move(&move, &board);
        } else {
            make_move(&move, &board);
        }

        // add move to move history
        if (move_history.length == move_history_capacity) {
            move_history_capacity *= 2;
            move_history.moves = realloc(move_history.moves, move_history_capacity * sizeof(Move));
        }
        move_history.moves[move_history.length++] = move;


        player_to_move = player_to_move == WHITE ? BLACK : WHITE;
    }

    free(move_history.moves);
    deallocate_board(&board);
}
