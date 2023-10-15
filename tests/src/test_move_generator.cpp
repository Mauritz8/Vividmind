#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "board.h"
#include "engine/engine.h"
#include "game_state.h"
#include "piece.h"
#include "move.h"


TEST(test_move_generation, test_move_generation_on_initial_position) {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; 
    Board board = Board::get_position_from_fen(fen);
    std::vector<Move> move_history;

    EXPECT_EQ(perft(1, board, move_history), 20);
    EXPECT_EQ(perft(2, board, move_history), 400);
}

TEST(test_move_generation, test_move_generation_on_position_2) {
    const std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"; 
    Board board = Board::get_position_from_fen(fen);
    std::vector<Move> move_history;

    EXPECT_EQ(perft(1, board, move_history), 48);
    EXPECT_EQ(perft(2, board, move_history), 2039);
}

TEST(test_move_generation, test_move_generation_on_position_3) {
    const std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"; 
    Board board = Board::get_position_from_fen(fen);
    std::vector<Move> move_history;

    EXPECT_EQ(perft(1, board, move_history), 14);
    EXPECT_EQ(perft(2, board, move_history), 191);
}

TEST(test_move_generation, test_move_generation_on_position_4) {
    const std::string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"; 
    Board board = Board::get_position_from_fen(fen);
    std::vector<Move> move_history;

    EXPECT_EQ(perft(1, board, move_history), 6);
    EXPECT_EQ(perft(2, board, move_history), 264);
}

TEST(test_move_generation, test_move_generation_on_position_5) {
    const std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"; 
    Board board = Board::get_position_from_fen(fen);
    std::vector<Move> move_history;

    EXPECT_EQ(perft(1, board, move_history), 44);
    EXPECT_EQ(perft(2, board, move_history), 1486);
    EXPECT_EQ(perft(3, board, move_history), 62379);
}

TEST(test_move_generation, test_move_generation_on_position_6) {
    const std::string fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"; 
    Board board = Board::get_position_from_fen(fen);
    std::vector<Move> move_history;

    EXPECT_EQ(perft(1, board, move_history), 46);
    EXPECT_EQ(perft(2, board, move_history), 2079);
}
