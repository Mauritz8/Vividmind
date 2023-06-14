#include <CUnit/Basic.h>

#include "test/test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"



static void test_rook_move_valid_move(void) {
    Board board;
    setup_empty_board(&board);
    Piece rook = {.piece_type = ROOK};
    place_piece_at(&rook, &board, 3, 4);

    Move move;
    move.start_square = &board.squares[4][3];

    move.end_square = &board.squares[1][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board));
    move.end_square = &board.squares[4][7];
    CU_ASSERT_TRUE(is_legal_move(&move, &board));
    move.end_square = &board.squares[5][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board));
    move.end_square = &board.squares[4][1];
    CU_ASSERT_TRUE(is_legal_move(&move, &board));

    deallocate_board(&board);
}

static void test_rook_move_invalid_pattern(void) {
    Board board;
    setup_empty_board(&board);
    Piece rook = {.piece_type = ROOK};
    place_piece_at(&rook, &board, 3, 3);

    Move move;
    move.start_square = &board.squares[3][3];

    move.end_square = &board.squares[2][4];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));
    move.end_square = &board.squares[5][5];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));
    move.end_square = &board.squares[6][2];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));
    move.end_square = &board.squares[4][0];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));
    move.end_square = &board.squares[1][0];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));

    deallocate_board(&board);
}

static void test_rook_move_jump_over_pieces(void) {
    Board board;
    setup_empty_board(&board);
    Piece rook = {.piece_type = ROOK};
    place_piece_at(&rook, &board, 3, 4);

    Piece blocking_piece = {.piece_type = PAWN};
    place_piece_at(&blocking_piece, &board, 3, 3);

    Move move;
    move.start_square = &board.squares[4][3];

    move.end_square = &board.squares[0][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board));
    move.end_square = &board.squares[1][3];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));
    move.end_square = &board.squares[2][3];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));

    deallocate_board(&board);
}

static void test_rook_move_same_color_on_target_square(void) {
    Board board;
    setup_empty_board(&board);
    Piece rook = {.piece_type = ROOK, .color = BLACK};
    place_piece_at(&rook, &board, 3, 4);

    Piece same_color_piece = {.piece_type = PAWN, .color = BLACK};
    place_piece_at(&same_color_piece, &board, 5, 4);

    Move move;
    move.start_square = &board.squares[4][3];
    move.end_square = &board.squares[4][5];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));

    deallocate_board(&board);
}

static void test_rook_move_capture_opponent_piece(void) {
    Board board;
    setup_empty_board(&board);
    Piece rook = {.piece_type = ROOK, .color = BLACK};
    place_piece_at(&rook, &board, 3, 4);

    Piece same_color_piece = {.piece_type = PAWN, .color = WHITE};
    place_piece_at(&same_color_piece, &board, 3, 7);

    Move move;
    move.start_square = &board.squares[4][3];
    move.end_square = &board.squares[7][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board));

    deallocate_board(&board);
}

static void test_rook_move_out_of_bounds(void) {
    Board board;
    setup_empty_board(&board);
    Piece rook = {.piece_type = ROOK};
    place_piece_at(&rook, &board, 6, 4);

    Move move;
    move.start_square = &board.squares[4][6];

    move.end_square = &board.squares[4][8];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));
    move.end_square = &board.squares[8][6];
    CU_ASSERT_FALSE(is_legal_move(&move, &board));

    deallocate_board(&board);
}

int rook_moves_tests(void) {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("rook_moves_test_suite", 0, 0);

    if ((NULL == CU_add_test(pSuite, "test_rook_move_valid_move", test_rook_move_valid_move)) ||
         NULL == CU_add_test(pSuite, "test_rook_move_invalid_pattern", test_rook_move_invalid_pattern) ||
         NULL == CU_add_test(pSuite, "test_rook_move_jump_over_pieces", test_rook_move_jump_over_pieces) ||
         NULL == CU_add_test(pSuite, "test_rook_move_same_color_on_target_square", test_rook_move_same_color_on_target_square) ||
         NULL == CU_add_test(pSuite, "test_rook_move_capture_opponent_piece", test_rook_move_capture_opponent_piece) ||
         NULL == CU_add_test(pSuite, "test_rook_move_out_of_bounds", test_rook_move_out_of_bounds))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return 0;
}
