#include "board.h"

int main(void) {
    struct Board board;

    setup_board(&board);
    show_board(&board);

    deallocate_board(&board);
}
