#include <stdio.h>
#include <stdbool.h>

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
            int start_x, start_y, end_x, end_y;
            printf("start_x: ");
            scanf("%d", &start_x);
            printf("start_y: ");
            scanf("%d", &start_y);
            printf("end_x: ");
            scanf("%d", &end_x);
            printf("end_y: ");
            scanf("%d", &end_y);

            move.start_square = &board.squares[start_y][start_x];
            move.end_square = &board.squares[end_y][end_x];

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
