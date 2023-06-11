#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "move.h"

int main(void) {
    Board board;
    setup_board(&board);

    while (true) {
        show_board(&board);

        int start_x, start_y, end_x, end_y;
        printf("start_x: ");
        scanf("%d", &start_x);
        printf("start_y: ");
        scanf("%d", &start_y);
        printf("end_x: ");
        scanf("%d", &end_x);
        printf("end_y: ");
        scanf("%d", &end_y);

        Move move;
        move.start_square = &board.squares[start_y][start_x];
        move.end_square = &board.squares[end_y][end_x];
        if (is_valid_move(&move, &board)) {
            make_move(&move);
        }
    }

    deallocate_board(&board);
}
