#include <gtest/gtest.h>
#include "test_move.cpp"
#include "test_gen_pseudo_legal_moves.cpp"
#include "test_draw.cpp"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
