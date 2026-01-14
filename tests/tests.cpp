#include <gtest/gtest.h>

#include "test_board.cpp"
#include "test_draw.cpp"
#include "test_move.cpp"
#include "test_basic_move_gen.cpp"
#include "test_move_sort.cpp"
#include "test_move_gen.cpp"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
