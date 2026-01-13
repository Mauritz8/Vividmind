#include "defs.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include "move.hpp"
#include <gtest/gtest.h>

static void assertMoveListsEqual(std::vector<Move> actual_moves,
                                 std::vector<Move> expected_moves) {
  EXPECT_EQ(actual_moves.size(), expected_moves.size());
  for (Move expected_move : expected_moves) {
    EXPECT_TRUE(std::find(actual_moves.begin(), actual_moves.end(),
                          expected_move) != actual_moves.end())
        << fmt::format("move {} not generated",
                       expected_move.to_uci_notation());
  }

  for (Move actual_move : actual_moves) {
    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(),
                          actual_move) != expected_moves.end())
        << fmt::format("move {} generated but shouldn't be",
                       actual_move.to_uci_notation());
  }
}

TEST(PseudoLegalMoveGenTests, FindAllPseudoLegalMovesTest1) {
  Board board = fen::get_position(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

  std::vector<Move> actual_moves = board.get_pseudo_legal_moves();
  std::vector<Move> expected_moves = {
      Move(d5, d6), Move(d5, e6),

      Move(e5, c6), Move(e5, d7), Move(e5, f7), Move(e5, g6), Move(e5, g4),
      Move(e5, d3), Move(e5, c4),

      Move(c3, a4), Move(c3, b5), Move(c3, d1), Move(c3, b1),

      Move(f3, d3), Move(f3, e3), Move(f3, f4), Move(f3, f5), Move(f3, f6),
      Move(f3, g3), Move(f3, h3), Move(f3, g4), Move(f3, h5),

      Move(a2, a3), Move(a2, a4), Move(b2, b3),

      Move(d2, c1), Move(d2, e3), Move(d2, f4), Move(d2, g5), Move(d2, h6),

      Move(e2, f1), Move(e2, d3), Move(e2, c4), Move(e2, b5), Move(e2, a6),
      Move(e2, d1),

      Move(g2, g3), Move(g2, g4), Move(g2, h3),

      Move(a1, b1), Move(a1, c1), Move(a1, d1),

      Move(e1, f1), Move(e1, d1), Move(e1, g1), Move(e1, c1),

      Move(h1, g1), Move(h1, f1),
  };

  assertMoveListsEqual(actual_moves, expected_moves);
}

TEST(PseudoLegalMoveGenTests, FindTacticalPseudoLegalMovesTest1) {
  Board board = fen::get_position(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

  std::vector<Move> actual_moves = board.get_pseudo_legal_moves(true);
  std::vector<Move> expected_moves = {
      Move(d5, e6),

      Move(e5, d7), Move(e5, f7), Move(e5, g6),

      Move(f3, f6), Move(f3, h3),

      Move(e2, a6),

      Move(g2, h3),
  };

  assertMoveListsEqual(actual_moves, expected_moves);
}

TEST(PseudoLegalMoveGenTests, FindAllPseudoLegalMovesTest2) {
  Board board = fen::get_position(
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

  std::vector<Move> actual_moves = board.get_pseudo_legal_moves();
  std::vector<Move> expected_moves = {
      Move(d7, c8, QUEEN),  Move(d7, c8, ROOK), Move(d7, c8, KNIGHT),
      Move(d7, c8, BISHOP),

      Move(c4, b3),         Move(c4, b5),       Move(c4, a6),
      Move(c4, d5),         Move(c4, e6),       Move(c4, f7),
      Move(c4, d3),

      Move(a2, a3),         Move(a2, a4),       Move(b2, b3),
      Move(b2, b4),         Move(c2, c3),

      Move(e2, c3),         Move(e2, d4),       Move(e2, f4),
      Move(e2, g3),         Move(e2, g1),

      Move(g2, g3),         Move(g2, g4),       Move(h2, h3),
      Move(h2, h4),

      Move(b1, a3),         Move(b1, c3),       Move(b1, d2),

      Move(c1, d2),         Move(c1, e3),       Move(c1, f4),
      Move(c1, g5),         Move(c1, h6),

      Move(d1, d2),         Move(d1, d3),       Move(d1, d4),
      Move(d1, d5),         Move(d1, d6),

      Move(e1, d2),         Move(e1, f2),       Move(e1, f1),
      Move(e1, g1),

      Move(h1, g1),         Move(h1, f1),
  };

  assertMoveListsEqual(actual_moves, expected_moves);
}

TEST(PseudoLegalMoveGenTests, FindTacticalPseudoLegalMovesTest2) {
  Board board = fen::get_position(
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

  std::vector<Move> actual_moves = board.get_pseudo_legal_moves(true);
  std::vector<Move> expected_moves = {
      Move(d7, c8, QUEEN),  Move(d7, c8, ROOK), Move(d7, c8, KNIGHT),
      Move(d7, c8, BISHOP), Move(c4, f7),       Move(e1, f2),
  };

  assertMoveListsEqual(actual_moves, expected_moves);
}

TEST(PseudoLegalMoveGenTests, FindPseudoLegalMovesTest3) {
  Board board = fen::get_position("8/3k4/4r3/8/5N2/3K4/8/8 w - - 0 1");

  std::vector<Move> actual_moves = board.get_pseudo_legal_moves();
  std::vector<Move> expected_moves = {
      Move(d3, c4), Move(d3, c3), Move(d3, c2), Move(d3, d4),
      Move(d3, d2), Move(d3, e4), Move(d3, e3), Move(d3, e2),

      Move(f4, d5), Move(f4, e6), Move(f4, g6), Move(f4, h5),
      Move(f4, h3), Move(f4, g2), Move(f4, e2),
  };
  assertMoveListsEqual(actual_moves, expected_moves);

  std::vector<Move> actual_tactical_moves = board.get_pseudo_legal_moves(true);
  std::vector<Move> expected_tactical_moves = {Move(f4, e6)};
  assertMoveListsEqual(actual_tactical_moves, expected_tactical_moves);
}
