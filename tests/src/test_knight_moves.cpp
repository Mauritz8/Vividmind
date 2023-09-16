#include <gtest/gtest.h>
#include <vector>

#include "board.h"
#include "piece.h"
#include "move.h"


TEST(test_knight_moves, test_knight_move_valid_move) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    const std::vector<Move> move_history;
    const Piece knight = Piece(KNIGHT, BLACK);
    board.set_square(4, 4, knight);

    Move move;
    move.set_start_square(board.get_square(4, 4));

    move.set_end_square(board.get_square(5, 2));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(6, 3));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(6, 5));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(5, 6));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(3, 6));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(2, 5));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(2, 3));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(3, 2));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}

TEST(test_knight_moves, test_knight_move_invalid_pattern) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    const std::vector<Move> move_history;
    const Piece knight = Piece(KNIGHT, BLACK);
    board.set_square(3, 3, knight);

    Move move;
    move.set_start_square(board.get_square(3, 3));

    move.set_end_square(board.get_square(3, 1));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(3, 2));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(4, 3));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(5, 7));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(0, 3));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(2, 0));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
}

TEST(test_knight_moves, test_knight_move_same_color_on_target_square) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    const std::vector<Move> move_history;
    const Piece knight = Piece(KNIGHT, WHITE);
    const Piece piece_of_same_color = Piece(ROOK, WHITE);
    board.set_square(3, 3, knight);
    board.set_square(4, 5, piece_of_same_color);

    Move move = Move(board.get_square(3, 3), board.get_square(4, 5));
    EXPECT_FALSE(move.is_legal_move(board, move_history));
}

TEST(test_knight_moves, test_knight_move_capture_opponent_piece) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    const std::vector<Move> move_history;
    const Piece knight = Piece(KNIGHT, WHITE);
    const Piece piece_of_opposite_color = Piece(ROOK, BLACK);
    board.set_square(3, 3, knight);
    board.set_square(4, 5, piece_of_opposite_color);

    Move move = Move(board.get_square(3, 3), board.get_square(4, 5));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}

TEST(test_knight_moves, test_knight_move_on_edge_of_board) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(WHITE);
    const std::vector<Move> move_history;
    const Piece knight = Piece(KNIGHT, WHITE);
    board.set_square(7, 1, knight);

    Move move; 
    move.set_start_square(board.get_square(7, 1)); 
        
    move.set_end_square(board.get_square(5, 0));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(5, 2));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(6, 3));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}

TEST(test_knight_moves, test_knight_move_jump_over_pieces) {
    Board board = Board::get_empty_board();
    board.set_player_to_move(BLACK);
    const std::vector<Move> move_history;
    const Piece knight = Piece(KNIGHT, BLACK);
    const Piece blocking_piece1 = Piece(PAWN, BLACK);
    const Piece blocking_piece2 = Piece(PAWN, WHITE);
    board.set_square(4, 4, knight);
    board.set_square(5, 4, blocking_piece1);
    board.set_square(3, 5, blocking_piece2);

    Move move; 
    move.set_start_square(board.get_square(4, 4)); 
        
    move.set_end_square(board.get_square(3, 6));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
    move.set_end_square(board.get_square(6, 3));
    EXPECT_TRUE(move.is_legal_move(board, move_history));
}
