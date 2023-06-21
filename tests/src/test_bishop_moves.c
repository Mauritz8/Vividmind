#include <CUnit/Basic.h>

#include "test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"



static void test_bishop_move_valid_move(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece bishop = {.piece_type = BISHOP};
    place_piece_at(&bishop, &board, 3, 4);

    Move move;
    move.start_square = &board.squares[4][3];

    move.end_square = &board.squares[0][7];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[7][6];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[6][1];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][2];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));

    deallocate_board(&board);
}

static void test_bishop_move_jump_over_pieces(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece bishop = {.piece_type = BISHOP};
    place_piece_at(&bishop, &board, 3, 4);

    const Piece blocking_piece = {.piece_type = PAWN};
    place_piece_at(&blocking_piece, &board, 5, 2);

    Move move;
    move.start_square = &board.squares[4][3];

    move.end_square = &board.squares[3][4];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[1][6];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[0][7];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));

    deallocate_board(&board);
}

static void test_bishop_move_same_color_on_target_square(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece bishop = {.piece_type = BISHOP, .color = BLACK};
    place_piece_at(&bishop, &board, 3, 4);

    const Piece same_color_piece = {.piece_type = PAWN, .color = BLACK};
    place_piece_at(&same_color_piece, &board, 5, 2);

    Move move;
    move.start_square = &board.squares[4][3];
    move.end_square = &board.squares[2][5];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));

    deallocate_board(&board);
}

static void test_bishop_move_capture_opponent_piece(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece bishop = {.piece_type = BISHOP, .color = BLACK};
    place_piece_at(&bishop, &board, 3, 4);

    const Piece same_color_piece = {.piece_type = PAWN, .color = WHITE};
    place_piece_at(&same_color_piece, &board, 1, 6);

    Move move;
    move.start_square = &board.squares[4][3];
    move.end_square = &board.squares[6][1];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));

    deallocate_board(&board);
}

static void test_bishop_move_out_of_bounds(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece bishop = {.piece_type = BISHOP};
    place_piece_at(&bishop, &board, 6, 4);

    Move move;
    move.start_square = &board.squares[4][6];

    move.end_square = &board.squares[2][8];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[7][9];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));

    deallocate_board(&board);
}

int bishop_moves_tests(void) {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("bishop_moves_test_suite", 0, 0);

    if ((NULL == CU_add_test(pSuite, "test_bishop_move_valid_move", test_bishop_move_valid_move)) ||
         NULL == CU_add_test(pSuite, "test_bishop_move_jump_over_pieces", test_bishop_move_jump_over_pieces) ||
         NULL == CU_add_test(pSuite, "test_bishop_move_same_color_on_target_square", test_bishop_move_same_color_on_target_square) ||
         NULL == CU_add_test(pSuite, "test_bishop_move_capture_opponent_piece", test_bishop_move_capture_opponent_piece) ||
         NULL == CU_add_test(pSuite, "test_bishop_move_out_of_bounds", test_bishop_move_out_of_bounds))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return 0;
}
