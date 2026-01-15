#include "defs.hpp"
#include "engine/move_sort.hpp"
#include "fen.hpp"
#include "move.hpp"
#include <gtest/gtest.h>
#include <optional>
#include <unordered_set>

TEST(MoveSortTests, Position1) {
  Board board = fen::get_position(
      "r5k1/ppp3r1/3b2qp/PP1Ppp2/4n2B/1B1Q1P1P/6P1/2R1R1K1 w - - 1 29");
  std::vector<Move> moves = board.get_legal_moves();
  std::optional<Move> best_move_prev_depth = std::make_optional(Move(c1, c2));
  std::unordered_set<Move, Move::HashFunction> killer_moves = {};
  sort_moves(moves, best_move_prev_depth, killer_moves, board);

  EXPECT_EQ(moves.at(0), Move(c1, c2));
  EXPECT_EQ(moves.at(1), Move(f3, e4));
  EXPECT_EQ(moves.at(2), Move(e1, e4));
  EXPECT_EQ(moves.at(3), Move(c1, c7));
  EXPECT_EQ(moves.at(4), Move(d3, e4));
}

TEST(MoveSortTests, Position2) {
  Board board = fen::get_position(
      "r1bq1rk1/pp1nbpp1/4p2p/3pP3/1npP4/2P2N2/PPQ1NPPP/RBB2RK1 w - - 2 12");
  std::vector<Move> moves = board.get_legal_moves();
  std::unordered_set<Move, Move::HashFunction> killer_moves = {Move(c2, h7)};
  sort_moves(moves, std::nullopt, killer_moves, board);

  EXPECT_EQ(moves.at(0), Move(c3, b4));
  EXPECT_EQ(moves.at(1), Move(c2, h7));
}
