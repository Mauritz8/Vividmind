#include "board.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include <gtest/gtest.h>
#include <memory>

TEST(DrawTests, TestInsufficientMaterial) {
  std::vector<std::string> fens = {
    "8/8/3k4/8/8/3K4/8/8 w - - 0 1",
    "8/8/3k4/8/5N2/3K4/8/8 w - - 0 1",
    "8/8/3k4/8/1B6/3K4/8/8 w - - 0 1",
  };

  for (std::string fen : fens) {
    std::unique_ptr<Board> b = fen::get_position(fen);
    EXPECT_TRUE(b->is_insufficient_material())
        << fmt::format("{} should be draw by insufficent material", fen);
  }
}

TEST(DrawTests, TestNotInsufficientMaterial) {
  std::vector<std::string> fens = {
    "8/8/3k4/6Q1/8/3K4/8/8 w - - 0 1",
    "8/8/3k4/8/8/3K4/8/1R6 w - - 0 1",
    "8/8/3k4/8/8/3K4/1P6/8 w - - 0 1",
    "8/8/3k4/8/8/3K4/8/3BB3 w - - 0 1",
    "8/8/3k4/8/5NN1/3K4/8/8 w - - 0 1",
  };

  for (std::string fen : fens) {
    std::unique_ptr<Board> b = fen::get_position(fen);
    EXPECT_FALSE(b->is_insufficient_material())
        << fmt::format("{} should not be draw by insufficent material", fen);
  }
}

TEST(DrawTests, TestFiftyMoveRule) {
  std::unique_ptr<Board> b =
      fen::get_position("1r6/8/3k4/8/8/3K4/8/5R2 w - - 0 1");

  std::vector<Move> white_rook_moves = {
    Move(f1, f2),
    Move(f2, f3),
    Move(f3, f4),
    Move(f4, f5),
    Move(f5, f6),
    Move(f6, f7),
    Move(f7, f8),

    Move(f8, f7),
    Move(f7, f6),
    Move(f6, f5),
    Move(f5, f4),
    Move(f4, f3),
    Move(f3, f2),
    Move(f2, f1),
  };
  std::vector<Move> black_rook_moves = {
    Move(b8, b7),
    Move(b7, b6),
    Move(b6, b5),
    Move(b5, b4),
    Move(b4, b3),
    Move(b3, b2),
    Move(b2, b1),

    Move(b1, b2),
    Move(b2, b3),
    Move(b3, b4),
    Move(b4, b5),
    Move(b5, b6),
    Move(b6, b7),
    Move(b7, b8),
  };

  std::vector<Move> white_king_moves = {
    Move(d3, e3),
    Move(e3, e2),
    Move(e2, d2),
    Move(d2, c2),
  };

  std::vector<Move> black_king_moves = {
    Move(d6, e6),
    Move(e6, e7),
    Move(e7, d7),
    Move(d7, c7),
  };
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < white_rook_moves.size(); j++) {
      b->make(white_rook_moves.at(j));
      b->make(black_rook_moves.at(j));
    }
    b->make(white_king_moves.at(i));
    b->make(black_king_moves.at(i));
    if (i < 3) {
      EXPECT_FALSE(b->is_draw_by_fifty_move_rule());
    }
  }

  EXPECT_TRUE(b->is_draw_by_fifty_move_rule());
}

TEST(DrawTests, TestThreefoldRepetitionConsecutiveMoves) {
  std::unique_ptr<Board> b =
      fen::get_position("5k2/8/6r1/8/3Q4/8/4K3/8 w - - 0 1");

  std::vector<Move> moves = {
    Move(d4, d8),
    Move(f8, g7),
    Move(d8, d4),
    Move(g7, f8),
  };
  EXPECT_FALSE(b->is_threefold_repetition());
  for (Move m : moves) {
    b->make(m);
  }
  for (Move m : moves) {
    b->make(m);
  }
  EXPECT_TRUE(b->is_threefold_repetition());
}

TEST(DrawTests, TestThreefoldRepetitionNonConsecutiveMoves) {
  std::unique_ptr<Board> b =
      fen::get_position("5k2/8/6r1/8/3Q4/8/4K3/8 w - - 0 1");

  std::vector<Move> moves = {
    Move(d4, d8),
    Move(f8, g7),
    Move(d8, d4),
    Move(g7, f8),
  };
  EXPECT_FALSE(b->is_threefold_repetition());
  for (Move m : moves) {
    b->make(m);
  }
  b->make(Move(d4, c4));
  b->make(Move(g6, h6));

  b->make(Move(c4, d4));
  b->make(Move(h6, g6));
  EXPECT_TRUE(b->is_threefold_repetition());
}
