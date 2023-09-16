#include <gtest/gtest.h>
#include <vector>

#include "board.h"
#include "piece.h"
#include "move.h"


TEST(test_rook_moves, test_rook_move_valid_move) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    const std::vector<Move> move_history;
    const Piece rook = Piece(ROOK, WHITE);
    board.set_square(3, 4, rook);

    Move move;
    move.set_start_square(board.get_square(3, 4));

    move.set_end_square(board.get_square(3, 1));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(7, 4));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(3, 5));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(1, 4));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}

TEST(test_rook_moves, test_rook_move_invalid_pattern) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    const std::vector<Move> move_history;
    const Piece rook = Piece(ROOK, BLACK);
    board.set_square(3, 3, rook);

    Move move;
    move.set_start_square(board.get_square(3, 3));

    move.set_end_square(board.get_square(4, 2));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(5, 5));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(2, 6));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(0, 4));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(0, 1));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
}

TEST(test_rook_moves, test_rook_move_jump_over_pieces) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    const std::vector<Move> move_history;
    const Piece rook = Piece(ROOK, WHITE);
    const Piece blocking_piece = Piece(PAWN, BLACK);
    board.set_square(3, 4, rook);
    board.set_square(3, 3, blocking_piece);

    Move move;
    move.set_start_square(board.get_square(3, 4));

    move.set_end_square(board.get_square(3, 0));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(3, 1));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(3, 2));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
}

TEST(test_rook_moves, test_rook_move_same_color_on_target_square) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    const std::vector<Move> move_history;
    const Piece rook = Piece(ROOK, BLACK);
    const Piece piece_of_same_color = Piece(PAWN, BLACK);
    board.set_square(3, 4, rook);
    board.set_square(5, 4, piece_of_same_color);

    Move move = Move(board.get_square(3, 4), board.get_square(5, 4));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
}

TEST(test_rook_moves, test_rook_move_capture_opponent_piece) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    const std::vector<Move> move_history;
    const Piece rook = Piece(ROOK, BLACK);
    const Piece piece_of_opposite_color = Piece(PAWN, WHITE);
    board.set_square(3, 4, rook);
    board.set_square(3, 7, piece_of_opposite_color);

    Move move = Move(board.get_square(3, 4), board.get_square(3, 7));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}
