#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "test/test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"



static void test_bishop_move_valid_move() {
    struct Board board;
    setup_empty_board(&board);
    struct Piece bishop = {.piece_type = BISHOP};
    place_piece_at(&bishop, &board, 3, 4);

    struct Move move;
    move.start_square = &board.squares[4][3];

    move.end_square = &board.squares[0][7];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[7][6];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[6][1];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][2];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

static void test_bishop_move_jump_over_pieces() {
    struct Board board;
    setup_empty_board(&board);
    struct Piece bishop = {.piece_type = BISHOP};
    place_piece_at(&bishop, &board, 3, 4);

    struct Piece blocking_piece = {.piece_type = PAWN};
    place_piece_at(&blocking_piece, &board, 5, 2);

    struct Move move;
    move.start_square = &board.squares[4][3];

    move.end_square = &board.squares[3][4];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[1][6];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[0][7];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

static void test_bishop_move_same_color_on_target_square() {
    struct Board board;
    setup_empty_board(&board);
    struct Piece bishop = {.piece_type = BISHOP, .color = BLACK};
    place_piece_at(&bishop, &board, 3, 4);

    struct Piece same_color_piece = {.piece_type = PAWN, .color = BLACK};
    place_piece_at(&same_color_piece, &board, 5, 2);

    struct Move move;
    move.start_square = &board.squares[4][3];
    move.end_square = &board.squares[2][5];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

static void test_bishop_move_capture_opponent_piece() {
    struct Board board;
    setup_empty_board(&board);
    struct Piece bishop = {.piece_type = BISHOP, .color = BLACK};
    place_piece_at(&bishop, &board, 3, 4);

    struct Piece same_color_piece = {.piece_type = PAWN, .color = WHITE};
    place_piece_at(&same_color_piece, &board, 1, 6);

    struct Move move;
    move.start_square = &board.squares[4][3];
    move.end_square = &board.squares[6][1];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

static void test_bishop_move_out_of_bounds() {
    struct Board board;
    setup_empty_board(&board);
    struct Piece bishop = {.piece_type = BISHOP};
    place_piece_at(&bishop, &board, 6, 4);

    struct Move move;
    move.start_square = &board.squares[4][6];

    move.end_square = &board.squares[2][8];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[7][9];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

int bishop_moves_tests() {
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
