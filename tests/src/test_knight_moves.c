#include <CUnit/Basic.h>

#include "game_state.h"
#include "test_utils.h"
#include "board.h"
#include "piece.h"
#include "move.h"



static void test_knight_move_valid_move(void) {
    Board board;
    setup_empty_board(&board);
    board.player_to_move = BLACK;
    MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT, .color = BLACK};
    place_piece_at(&knight, &board, 4, 4);

    Move move;
    move.start_square = &board.squares[4][4];

    move.end_square = &board.squares[2][5];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][6];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[5][6];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[6][5];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[6][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[5][2];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][2];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[2][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));

    deallocate_game_resources(&board, &move_history);
}

static void test_knight_move_invalid_pattern(void) {
    Board board;
    setup_empty_board(&board);
    MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT};
    place_piece_at(&knight, &board, 3, 3);

    Move move;
    move.start_square = &board.squares[3][3];

    move.end_square = &board.squares[1][3];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[2][3];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][4];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[7][5];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][0];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[0][2];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));

    deallocate_game_resources(&board, &move_history);
}

static void test_knight_move_same_color_on_target_square(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    const Piece piece_of_same_color = {.piece_type = ROOK, .color = WHITE};
    place_piece_at(&knight, &board, 3, 3);
    place_piece_at(&piece_of_same_color, &board, 4, 5);

    Move move;
    move.start_square = &board.squares[3][3];
    move.end_square = &board.squares[5][4];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
}

void test_knight_move_capture_opponent_piece(void) {
    Board board;
    setup_empty_board(&board);
    board.player_to_move = WHITE;
    const MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    const Piece piece_of_opposite_color = {.piece_type = ROOK, .color = BLACK};
    place_piece_at(&knight, &board, 3, 3);
    place_piece_at(&piece_of_opposite_color, &board, 4, 5);

    Move move;
    move.start_square = &board.squares[3][3];
    move.end_square = &board.squares[5][4];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
}

static void test_knight_move_out_of_bounds(void) {
    Board board;
    setup_empty_board(&board);
    const MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    place_piece_at(&knight, &board, 4, 4);

    Move move;
    move.start_square = &board.squares[4][4];
    move.end_square = &board.squares[9][3];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
}

static void test_knight_move_on_edge_of_board(void) {
    Board board;
    setup_empty_board(&board);
    board.player_to_move = WHITE;
    const MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT, .color = WHITE};
    place_piece_at(&knight, &board, 7, 1);

    Move move;
    move.start_square = &board.squares[1][7];

    move.end_square = &board.squares[0][5];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[2][5];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][6];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][8];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[2][9];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[0][9];
    CU_ASSERT_FALSE(is_legal_move(&move, &board, &move_history));
}

static void test_knight_move_jump_over_pieces(void) {
    Board board;
    setup_empty_board(&board);
    board.player_to_move = BLACK;
    MoveArray move_history = create_empty_move_history();
    const Piece knight = {.piece_type = KNIGHT, .color = BLACK};
    place_piece_at(&knight, &board, 4, 4);

    const Piece blocking_piece1 = {.piece_type = PAWN};
    place_piece_at(&blocking_piece1, &board, 5, 4);
    const Piece blocking_piece2 = {.piece_type = PAWN};
    place_piece_at(&blocking_piece2, &board, 3, 5);

    Move move;
    move.start_square = &board.squares[4][4];

    move.end_square = &board.squares[6][3];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));
    move.end_square = &board.squares[3][6];
    CU_ASSERT_TRUE(is_legal_move(&move, &board, &move_history));

    deallocate_game_resources(&board, &move_history);
}

int knight_moves_tests(void) {
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
