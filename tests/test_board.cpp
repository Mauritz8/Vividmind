#include "board/board.hpp"
#include "fen.hpp"
#include <gtest/gtest.h>

TEST(Board, get_doubled_pawns) {
  Board b = Board::get_starting_position();
  EXPECT_EQ(b.get_doubled_pawns(WHITE), 0);
  EXPECT_EQ(b.get_doubled_pawns(BLACK), 0);

  b = fen::get_position(
      "r5k1/1bp1q1bN/3p2p1/2p1P3/1nP2r2/1P6/1B2Q1P1/1K1R1B1R b - - 0 21");
  EXPECT_EQ(b.get_doubled_pawns(WHITE), 0);
  EXPECT_EQ(b.get_doubled_pawns(BLACK), 1);

  b = fen::get_position("1r6/R1pk2pp/2p2p2/2P5/8/1bP1BP2/6rP/R1K5 b - - 0 27");
  EXPECT_EQ(b.get_doubled_pawns(WHITE), 1);
  EXPECT_EQ(b.get_doubled_pawns(BLACK), 1);
}
