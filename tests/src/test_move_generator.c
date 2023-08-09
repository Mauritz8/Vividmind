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

static void test_move_generation_on_position_2(void) {
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 48);
    
    deallocate_game_resources(board, &move_history);
}

static void test_move_generation_on_position_3(void) {
    char fen[] = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 14);
    
    deallocate_game_resources(board, &move_history);
}


int move_generator_tests(void) {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("move_generator_test_suite", 0, 0);

    if (NULL == CU_add_test(pSuite, "test_move_generation_on_initial_position", test_move_generation_on_initial_position) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_2", test_move_generation_on_position_2) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_3", test_move_generation_on_position_3))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return 0;
}