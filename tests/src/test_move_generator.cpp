#include <gtest/gtest.h>
#include <string>

#include "board.h"
#include "board_helper.h"
#include "move_generator.h"

class MoveGenerationTest : public testing::Test {
    public:
        Board board;
        BoardHelper board_helper;
        MoveGenerator move_gen;

        MoveGenerationTest() 
            : board(Board())
            , board_helper(board)
            , move_gen(board, board_helper)
        {}
};

TEST_F(MoveGenerationTest, test_move_generation_on_initial_position) {
    board = Board::get_starting_position();

    EXPECT_EQ(move_gen.perft(1), 20);
    EXPECT_EQ(move_gen.perft(2), 400);
    EXPECT_EQ(move_gen.perft(3), 8902);
}

TEST_F(MoveGenerationTest, test_move_generation_on_position_2) {
    const std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"; 
    board = Board::get_position_from_fen(fen);

    EXPECT_EQ(move_gen.perft(1), 48);
    EXPECT_EQ(move_gen.perft(2), 2039);
    EXPECT_EQ(move_gen.perft(3), 97862);
}

TEST_F(MoveGenerationTest, test_move_generation_on_position_3) {
    const std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"; 
    board = Board::get_position_from_fen(fen);

    EXPECT_EQ(move_gen.perft(1), 14);
    EXPECT_EQ(move_gen.perft(2), 191);
    EXPECT_EQ(move_gen.perft(3), 2812);
}

TEST_F(MoveGenerationTest, test_move_generation_on_position_4) {
    const std::string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"; 
    board = Board::get_position_from_fen(fen);

    EXPECT_EQ(move_gen.perft(1), 6);
    EXPECT_EQ(move_gen.perft(2), 264);
    EXPECT_EQ(move_gen.perft(3), 9467);
}

TEST_F(MoveGenerationTest, test_move_generation_on_position_5) {
    const std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"; 
    board = Board::get_position_from_fen(fen);

    EXPECT_EQ(move_gen.perft(1), 44);
    EXPECT_EQ(move_gen.perft(2), 1486);
    EXPECT_EQ(move_gen.perft(3), 62379);
}

TEST_F(MoveGenerationTest, test_move_generation_on_position_6) {
    const std::string fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"; 
    board = Board::get_position_from_fen(fen);

    EXPECT_EQ(move_gen.perft(1), 46);
    EXPECT_EQ(move_gen.perft(2), 2079);
    EXPECT_EQ(move_gen.perft(3), 89890);
}
