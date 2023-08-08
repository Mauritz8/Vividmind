#include <CUnit/Basic.h>

#include "game_state.h"
#include "test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"


int perft(int depth, Board* board, MoveArray* move_history) {
    if (depth == 0) {
        return 1;
    }

    int nodes = 0;
    MoveArray move_list = get_all_legal_moves(board, move_history);
    for (int i = 0; i < move_list.length; i++) {
        make_appropriate_move(&move_list.moves[i], board, move_history);
        nodes += perft(depth - 1, board, move_history);    
        undo_appropriate_move(&move_list.moves[i], board, move_history);
    }
    return nodes;
}

static void test_move_generation_on_initial_position(void) {
    Board board;
    setup_board(&board);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, &board, &move_history), 20);
    CU_ASSERT_EQUAL(perft(2, &board, &move_history), 400);

    deallocate_game_resources(&board, &move_history);
}


int move_generator_tests(void) {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("move_generator_test_suite", 0, 0);

    if ((NULL == CU_add_test(pSuite, "test_move_generation_on_initial_position", test_move_generation_on_initial_position)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return 0;
}