#include "test_board.cpp"
#include "test_draw.cpp"
#include "test_gen_pseudo_legal_moves.cpp"
#include "test_move.cpp"
#include "test_move_gen.cpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
