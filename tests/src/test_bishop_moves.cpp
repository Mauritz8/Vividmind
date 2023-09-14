#include <gtest/gtest.h>
#include <vector>

#include "board.h"
#include "piece.h"
#include "move.h"


TEST(test_bishop_moves, test_bishop_move_valid_move) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    std::vector<Move> move_history;
    const Piece bishop = Piece(BISHOP, WHITE);
    board.set_square(3, 4, bishop);

    Move move;
    move.set_start_square(board.get_square(3, 4));

    move.set_end_square(board.get_square(7, 0));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(6, 7));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(1, 6));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(2, 3));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}

TEST(test_bishop_moves, test_bishop_move_jump_over_pieces) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    std::vector<Move> move_history;
    const Piece bishop = Piece(BISHOP, WHITE);
    const Piece blocking_piece = Piece(PAWN, BLACK);
    board.set_square(3, 4, bishop);
    board.set_square(2, 5, blocking_piece);

    Move move;
    move.set_start_square(board.get_square(3, 4));

    move.set_end_square(board.get_square(4, 3));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(6, 1));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(7, 0));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}

TEST(test_bishop_moves, test_bishop_move_same_color_on_target_square) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    std::vector<Move> move_history;
    const Piece bishop = Piece(BISHOP, BLACK);
    const Piece same_color_piece = Piece(PAWN, BLACK);
    board.set_square(3, 4, bishop);
    board.set_square(5, 2, same_color_piece);

    Move move;
    move.set_start_square(board.get_square(3, 4));
    move.set_end_square(board.get_square(5, 2));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
}

TEST(test_bishop_moves, test_bishop_move_capture_opponent_piece) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    std::vector<Move> move_history;
    const Piece bishop = Piece(BISHOP, BLACK);
    const Piece opponent_piece = Piece(PAWN, WHITE);
    board.set_square(3, 4, bishop);
    board.set_square(1, 6, opponent_piece);

    Move move;
    move.set_start_square(board.get_square(3, 4));
    move.set_end_square(board.get_square(1, 6));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}
