#include "defs.hpp"
#include "engine/move_sort.hpp"
#include "fen.hpp"
#include "move.hpp"
#include <gtest/gtest.h>
#include <optional>

TEST(MoveSortTests, Position1) {
  Board board = fen::get_position(
      "r5k1/ppp3r1/3b2qp/PP1Ppp2/4n2B/1B1Q1P1P/6P1/2R1R1K1 w - - 1 29");
  std::vector<Move> moves = board.get_legal_moves();
  sort_moves(moves, std::make_optional(Move(c1, c2)), board);

  EXPECT_EQ(moves.at(0), Move(c1, c2));
  EXPECT_EQ(moves.at(1), Move(f3, e4));
  EXPECT_EQ(moves.at(2), Move(e1, e4));
  EXPECT_EQ(moves.at(3), Move(c1, c7));
  EXPECT_EQ(moves.at(4), Move(d3, e4));
}
