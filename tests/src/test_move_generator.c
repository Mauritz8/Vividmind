#include <CUnit/Basic.h>

#include "game_state.h"
#include "test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"


static int perft(int depth, Board* board, MoveArray* move_history) {
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

static void divide(int depth, Board* board, MoveArray* move_history) {
    puts("");
    int nodes_searched = 0;
    MoveArray move_list = get_all_legal_moves(board, move_history);
    for (int i = 0; i < move_list.length; i++) {
        make_appropriate_move(&move_list.moves[i], board, move_history);
        int nodes =  perft(depth - 1, board, move_history);
        nodes_searched += nodes;
        printf("%s: %d\n", move_to_uci_notation(&move_list.moves[i]), nodes);
        undo_appropriate_move(&move_list.moves[i], board, move_history);
    }
    printf("\nNodes searched: %d\n", nodes_searched);
}

static void test_move_generation_on_initial_position(void) {
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 20);
    CU_ASSERT_EQUAL(perft(2, board, &move_history), 400);

    deallocate_game_resources(board, &move_history);
}

static void test_move_generation_on_position_2(void) {
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 48);
    CU_ASSERT_EQUAL(perft(2, board, &move_history), 2039);
    
    deallocate_game_resources(board, &move_history);
}

static void test_move_generation_on_position_3(void) {
    char fen[] = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 14);
    CU_ASSERT_EQUAL(perft(2, board, &move_history), 191);
    
    deallocate_game_resources(board, &move_history);
}

static void test_move_generation_on_position_4(void) {
    char fen[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 6);
    CU_ASSERT_EQUAL(perft(2, board, &move_history), 264);
    
    deallocate_game_resources(board, &move_history);
}

static void test_move_generation_on_position_5(void) {
    char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 44);
    CU_ASSERT_EQUAL(perft(2, board, &move_history), 1486);
    CU_ASSERT_EQUAL(perft(3, board, &move_history), 62379);
    
    deallocate_game_resources(board, &move_history);
}

static void test_move_generation_on_position_6(void) {
    char fen[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    Board* board = get_position_from_fen(fen);
    MoveArray move_history = create_empty_move_history();

    CU_ASSERT_EQUAL(perft(1, board, &move_history), 46);
    CU_ASSERT_EQUAL(perft(2, board, &move_history), 2079);

    deallocate_game_resources(board, &move_history);
}


int move_generator_tests(void) {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("move_generator_test_suite", 0, 0);

    if (
        NULL == CU_add_test(pSuite, "test_move_generation_on_initial_position", test_move_generation_on_initial_position) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_2", test_move_generation_on_position_2) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_3", test_move_generation_on_position_3) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_4", test_move_generation_on_position_4) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_5", test_move_generation_on_position_5) ||
        NULL == CU_add_test(pSuite, "test_move_generation_on_position_6", test_move_generation_on_position_6)
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return 0;
}