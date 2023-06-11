#include <CUnit/Basic.h>

#include "test/test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"



static void test_knight_move_valid_move() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT};
    place_piece_at(&knight, &board, 4, 4);

    Move move;
    move.start_square = &board.squares[4][4];

    move.end_square = &board.squares[2][5];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][6];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[5][6];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[6][5];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[6][3];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[5][2];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][2];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[2][3];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

static void test_knight_move_invalid_pattern() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT};
    place_piece_at(&knight, &board, 3, 3);

    Move move;
    move.start_square = &board.squares[3][3];

    move.end_square = &board.squares[1][3];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[2][3];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][4];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[7][5];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][0];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[0][2];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

static void test_knight_move_same_color_on_target_square() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    Piece piece_of_same_color = {.piece_type = ROOK, .color = WHITE};
    place_piece_at(&knight, &board, 3, 3);
    place_piece_at(&piece_of_same_color, &board, 4, 5);

    Move move;
    move.start_square = &board.squares[3][3];
    move.end_square = &board.squares[5][4];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
}

void test_knight_move_capture_opponent_piece() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    Piece piece_of_opposite_color = {.piece_type = ROOK, .color = BLACK};
    place_piece_at(&knight, &board, 3, 3);
    place_piece_at(&piece_of_opposite_color, &board, 4, 5);

    Move move;
    move.start_square = &board.squares[3][3];
    move.end_square = &board.squares[5][4];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
}

static void test_knight_move_out_of_bounds() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    place_piece_at(&knight, &board, 4, 4);

    Move move;
    move.start_square = &board.squares[4][4];
    move.end_square = &board.squares[9][3];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
}

static void test_knight_move_on_edge_of_board() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    place_piece_at(&knight, &board, 7, 1);

    Move move;
    move.start_square = &board.squares[1][7];

    move.end_square = &board.squares[0][5];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[2][5];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][6];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][8];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[2][9];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
    move.end_square = &board.squares[0][9];
    CU_ASSERT_FALSE(is_valid_move(&move, &board));
}

static void test_knight_move_jump_over_pieces() {
    Board board;
    setup_empty_board(&board);
    Piece knight = {.piece_type = KNIGHT};
    place_piece_at(&knight, &board, 4, 4);

    Piece blocking_piece1 = {.piece_type = PAWN};
    place_piece_at(&blocking_piece1, &board, 5, 4);
    Piece blocking_piece2 = {.piece_type = PAWN};
    place_piece_at(&blocking_piece2, &board, 3, 5);

    Move move;
    move.start_square = &board.squares[4][4];

    move.end_square = &board.squares[6][3];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));
    move.end_square = &board.squares[3][6];
    CU_ASSERT_TRUE(is_valid_move(&move, &board));

    deallocate_board(&board);
}

int knight_moves_tests() {
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("knight_moves_test_suite", 0, 0);

    if ((NULL == CU_add_test(pSuite, "test_knight_move_valid_move", test_knight_move_valid_move)) ||
        (NULL == CU_add_test(pSuite, "test_knight_move_invalid_pattern", test_knight_move_invalid_pattern)) ||
        (NULL == CU_add_test(pSuite, "test_knight_move_same_color_on_target_square", test_knight_move_same_color_on_target_square)) ||
        (NULL == CU_add_test(pSuite, "test_knight_move_capture_opponent_piece", test_knight_move_capture_opponent_piece)) ||
        (NULL == CU_add_test(pSuite, "test_knight_move_out_of_bounds", test_knight_move_out_of_bounds)) ||
        (NULL == CU_add_test(pSuite, "test_knight_move_on_edge_of_board", test_knight_move_on_edge_of_board)) ||
        (NULL == CU_add_test(pSuite, "test_knight_move_jump_over_pieces", test_knight_move_jump_over_pieces))) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return 0;
}
