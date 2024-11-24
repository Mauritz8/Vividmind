#include "board.hpp"
#include "fen.hpp"
#include "fmt/core.h"
#include "perft.hpp"
#include <gtest/gtest.h>
#include <memory>

static void test_fen(std::string_view fen,
                 const std::vector<int> &expected_moves_list) {
  std::unique_ptr<Board> b = fen::get_position(fen);
  for (int i = 0; i < expected_moves_list.size(); i++) {
    int depth = i + 1;
    int expected_moves = expected_moves_list.at(i);
    int actual_moves = perft(b, depth);
    EXPECT_EQ(actual_moves, expected_moves)
        << fmt::format("FEN: {} (depth {} found {}/{} moves)", fen, depth,
                       actual_moves, expected_moves);
  }
}

// from http://www.rocechess.ch/perft.html
TEST(MoveGenTests, Position1) {
  std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  std::vector<int> expected = {20, 400, 8902, 197281};
  test_fen(fen, expected);
}

TEST(MoveGenTests, Position2) {
  std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  std::vector<int> expected = {48, 2039, 97862};
  test_fen(fen, expected);
}

TEST(MoveGenTests, Position3) {
  std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
  std::vector<int> expected = {15, 66, 1197, 7059, 133987, 764643};
  test_fen(fen, expected);
}

TEST(MoveGenTests, Position4) {
  std::string fen = "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1";
  std::vector<int> expected = {16, 71, 1287, 7626, 145232, 846648};
  test_fen(fen, expected);
}

TEST(MoveGenTests, Position5) {
  std::string fen = "4k2r/8/8/8/8/8/8/4K3 w k - 0 1";
  std::vector<int> expected = {5, 75, 459, 8290, 47635, 899442};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position6) {
  std::string fen = "r3k3/8/8/8/8/8/8/4K3 w q - 0 1";
  std::vector<int> expected = {5, 80, 493, 8897, 52710, 1001523};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position7) {
  std::string fen = "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1";
  std::vector<int> expected = {26, 112, 3189, 17945, 532933};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position8) {
  std::string fen = "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1";
  std::vector<int> expected = {5, 130, 782, 22180, 118882};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position9) {
  std::string fen = "8/8/8/8/8/8/6k1/4K2R w K - 0 1";
  std::vector<int> expected = {12, 38, 564, 2219, 37735, 185867};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position10) {
  std::string fen = "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1";
  std::vector<int> expected = {15, 65, 1018, 4573, 80619, 413018};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position11) {
  std::string fen = "4k2r/6K1/8/8/8/8/8/8 w k - 0 1";
  std::vector<int> expected = {3, 32, 134, 2073, 10485, 179869};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position12) {
  std::string fen = "r3k3/1K6/8/8/8/8/8/8 w q - 0 1";
  std::vector<int> expected = {4, 49, 243, 3991, 20780, 367724};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position13) {
  std::string fen = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
  std::vector<int> expected = {26, 568, 13744, 314346};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position14) {
  std::string fen = "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1";
  std::vector<int> expected = {25, 567, 14095, 328965};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position15) {
  std::string fen = "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1";
  std::vector<int> expected = {25, 548, 13502, 312835};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position16) {
  std::string fen = "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1";
  std::vector<int> expected = {25, 547, 13579, 316214};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position17) {
  std::string fen = "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
  std::vector<int> expected = {26, 583, 14252, 334705};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position18) {
  std::string fen = "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1";
  std::vector<int> expected = {25, 560, 13592, 317324};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position19) {
  std::string fen = "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1";
  std::vector<int> expected = {25, 560, 13607, 320792};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position20) {
  std::string fen = "4k3/8/8/8/8/8/8/4K2R b K - 0 1";
  std::vector<int> expected = {5, 75, 459, 8290, 47635, 899442};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position21) {
  std::string fen = "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1";
  std::vector<int> expected = {5, 80, 493, 8897, 52710, 1001523};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position22) {
  std::string fen = "4k2r/8/8/8/8/8/8/4K3 b k - 0 1";
  std::vector<int> expected = {15, 66, 1197, 7059, 133987, 764643};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position23) {
  std::string fen = "r3k3/8/8/8/8/8/8/4K3 b q - 0 1";
  std::vector<int> expected = {16, 71, 1287, 7626, 145232, 846648};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position24) {
  std::string fen = "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1";
  std::vector<int> expected = {5, 130, 782, 22180, 118882};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position25) {
  std::string fen = "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1";
  std::vector<int> expected = {26, 112, 3189, 17945, 532933};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position26) {
  std::string fen = "8/8/8/8/8/8/6k1/4K2R b K - 0 1";
  std::vector<int> expected = {3, 32, 134, 2073, 10485, 179869};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position27) {
  std::string fen = "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1";
  std::vector<int> expected = {4, 49, 243, 3991, 20780, 367724};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position28) {
  std::string fen = "4k2r/6K1/8/8/8/8/8/8 b k - 0 1";
  std::vector<int> expected = {12, 38, 564, 2219, 37735, 185867};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position29) {
  std::string fen = "r3k3/1K6/8/8/8/8/8/8 b q - 0 1";
  std::vector<int> expected = {15, 65, 1018, 4573, 80619, 413018};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position30) {
  std::string fen = "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1";
  std::vector<int> expected = {26, 568, 13744, 314346};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position31) {
  std::string fen = "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1";
  std::vector<int> expected = {26, 583, 14252, 334705};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position32) {
  std::string fen = "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1";
  std::vector<int> expected = {25, 560, 13592, 317324};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position33) {
  std::string fen = "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1";
  std::vector<int> expected = {25, 560, 13607, 320792};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position34) {
  std::string fen = "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1";
  std::vector<int> expected = {25, 567, 14095, 328965};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position35) {
  std::string fen = "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1";
  std::vector<int> expected = {25, 548, 13502, 312835};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position36) {
  std::string fen = "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1";
  std::vector<int> expected = {25, 547, 13579, 316214};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position37) {
  std::string fen = "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1";
  std::vector<int> expected = {14, 195, 2760, 38675, 570726};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position38) {
  std::string fen = "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1";
  std::vector<int> expected = {11, 156, 1636, 20534, 223507};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position39) {
  std::string fen = "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1";
  std::vector<int> expected = {19, 289, 4442, 73584};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position40) {
  std::string fen = "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1";
  std::vector<int> expected = {3, 51, 345, 5301, 38348, 588695};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position41) {
  std::string fen = "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1";
  std::vector<int> expected = {17, 54, 835, 5910, 92250, 688780};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position42) {
  std::string fen = "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1";
  std::vector<int> expected = {15, 193, 2816, 40039, 582642};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position43) {
  std::string fen = "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1";
  std::vector<int> expected = {16, 180, 2290, 24640, 288141};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position44) {
  std::string fen = "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1";
  std::vector<int> expected = {4, 68, 1118, 16199, 281190};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position45) {
  std::string fen = "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1";
  std::vector<int> expected = {17, 54, 835, 5910, 92250, 688780};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position46) {
  std::string fen = "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1";
  std::vector<int> expected = {3, 51, 345, 5301, 38348, 588695};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position47) {
  std::string fen = "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1";
  std::vector<int> expected = {17, 278, 4607, 76778};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position48) {
  std::string fen = "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1";
  std::vector<int> expected = {21, 316, 5744, 93338};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position49) {
  std::string fen = "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1";
  std::vector<int> expected = {21, 144, 3242, 32955, 787524};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position50) {
  std::string fen = "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1";
  std::vector<int> expected = {7, 143, 1416, 31787, 310862};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position51) {
  std::string fen = "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1";
  std::vector<int> expected = {6, 106, 1829, 31151, 530585};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position52) {
  std::string fen = "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1";
  std::vector<int> expected = {17, 309, 5133, 93603};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position53) {
  std::string fen = "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1";
  std::vector<int> expected = {7, 143, 1416, 31787, 310862};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position54) {
  std::string fen = "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1";
  std::vector<int> expected = {21, 144, 3242, 32955, 787524};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position55) {
  std::string fen = "7k/RR6/8/8/8/8/rr6/7K w - - 0 1";
  std::vector<int> expected = {19, 275, 5300, 104342};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position56) {
  std::string fen = "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1";
  std::vector<int> expected = {36, 1027, 29215, 771461};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position57) {
  std::string fen = "7k/RR6/8/8/8/8/rr6/7K b - - 0 1";
  std::vector<int> expected = {19, 275, 5300, 104342};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position58) {
  std::string fen = "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1";
  std::vector<int> expected = {36, 1027, 29227, 771368};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position59) {
  std::string fen = "6kq/8/8/8/8/8/8/7K w - - 0 1";
  std::vector<int> expected = {2, 36, 143, 3637, 14893, 391507};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position60) {
  std::string fen = "6KQ/8/8/8/8/8/8/7k b - - 0 1";
  std::vector<int> expected = {2, 36, 143, 3637, 14893, 391507};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position61) {
  std::string fen = "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1";
  std::vector<int> expected = {6, 35, 495, 8349, 166741};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position62) {
  std::string fen = "6qk/8/8/8/8/8/8/7K b - - 0 1";
  std::vector<int> expected = {22, 43, 1015, 4167, 105749, 419369};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position63) {
  std::string fen = "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1";
  std::vector<int> expected = {6, 35, 495, 8349, 166741};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position64) {
  std::string fen = "8/8/8/8/8/K7/P7/k7 w - - 0 1";
  std::vector<int> expected = {3, 7, 43, 199, 1347, 6249};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position65) {
  std::string fen = "8/8/8/8/8/7K/7P/7k w - - 0 1";
  std::vector<int> expected = {3, 7, 43, 199, 1347, 6249};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position66) {
  std::string fen = "K7/p7/k7/8/8/8/8/8 w - - 0 1";
  std::vector<int> expected = {1, 3, 12, 80, 342, 2343};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position67) {
  std::string fen = "7K/7p/7k/8/8/8/8/8 w - - 0 1";
  std::vector<int> expected = {1, 3, 12, 80, 342, 2343};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position68) {
  std::string fen = "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1";
  std::vector<int> expected = {7, 35, 210, 1091, 7028, 34834};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position69) {
  std::string fen = "8/8/8/8/8/K7/P7/k7 b - - 0 1";
  std::vector<int> expected = {1, 3, 12, 80, 342, 2343};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position70) {
  std::string fen = "8/8/8/8/8/7K/7P/7k b - - 0 1";
  std::vector<int> expected = {1, 3, 12, 80, 342, 2343};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position71) {
  std::string fen = "K7/p7/k7/8/8/8/8/8 b - - 0 1";
  std::vector<int> expected = {3, 7, 43, 199, 1347, 6249};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position72) {
  std::string fen = "7K/7p/7k/8/8/8/8/8 b - - 0 1";
  std::vector<int> expected = {3, 7, 43, 199, 1347, 6249};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position73) {
  std::string fen = "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1";
  std::vector<int> expected = {5, 35, 182, 1091, 5408, 34822};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position74) {
  std::string fen = "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1";
  std::vector<int> expected = {2, 8, 44, 282, 1814, 11848};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position75) {
  std::string fen = "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1";
  std::vector<int> expected = {2, 8, 44, 282, 1814, 11848};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position76) {
  std::string fen = "8/8/7k/7p/7P/7K/8/8 w - - 0 1";
  std::vector<int> expected = {3, 9, 57, 360, 1969, 10724};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position77) {
  std::string fen = "8/8/k7/p7/P7/K7/8/8 w - - 0 1";
  std::vector<int> expected = {3, 9, 57, 360, 1969, 10724};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position78) {
  std::string fen = "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1";
  std::vector<int> expected = {5, 25, 180, 1294, 8296, 53138};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position79) {
  std::string fen = "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1";
  std::vector<int> expected = {8, 61, 483, 3213, 23599, 157093};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position80) {
  std::string fen = "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1";
  std::vector<int> expected = {8, 61, 411, 3213, 21637, 158065};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position81) {
  std::string fen = "k7/8/3p4/8/3P4/8/8/7K w - - 0 1";
  std::vector<int> expected = {4, 15, 90, 534, 3450, 20960};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position82) {
  std::string fen = "8/8/7k/7p/7P/7K/8/8 b - - 0 1";
  std::vector<int> expected = {3, 9, 57, 360, 1969, 10724};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position83) {
  std::string fen = "8/8/k7/p7/P7/K7/8/8 b - - 0 1";
  std::vector<int> expected = {3, 9, 57, 360, 1969, 10724};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position84) {
  std::string fen = "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1";
  std::vector<int> expected = {5, 25, 180, 1294, 8296, 53138};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position85) {
  std::string fen = "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1";
  std::vector<int> expected = {8, 61, 411, 3213, 21637, 158065};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position86) {
  std::string fen = "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1";
  std::vector<int> expected = {8, 61, 483, 3213, 23599, 157093};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position87) {
  std::string fen = "k7/8/3p4/8/3P4/8/8/7K b - - 0 1";
  std::vector<int> expected = {4, 15, 89, 537, 3309, 21104};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position88) {
  std::string fen = "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1";
  std::vector<int> expected = {4, 19, 117, 720, 4661, 32191};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position89) {
  std::string fen = "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1";
  std::vector<int> expected = {5, 19, 116, 716, 4786, 30980};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position90) {
  std::string fen = "k7/8/8/7p/6P1/8/8/K7 w - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position91) {
  std::string fen = "k7/8/7p/8/8/6P1/8/K7 w - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position92) {
  std::string fen = "k7/8/8/6p1/7P/8/8/K7 w - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position93) {
  std::string fen = "k7/8/6p1/8/8/7P/8/K7 w - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position94) {
  std::string fen = "k7/8/8/3p4/4p3/8/8/7K w - - 0 1";
  std::vector<int> expected = {3, 15, 84, 573, 3013, 22886};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position95) {
  std::string fen = "k7/8/3p4/8/8/4P3/8/7K w - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4271, 28662};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position96) {
  std::string fen = "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1";
  std::vector<int> expected = {5, 19, 117, 720, 5014, 32167};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position97) {
  std::string fen = "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1";
  std::vector<int> expected = {4, 19, 117, 712, 4658, 30749};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position98) {
  std::string fen = "k7/8/8/7p/6P1/8/8/K7 b - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position99) {
  std::string fen = "k7/8/7p/8/8/6P1/8/K7 b - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position100) {
  std::string fen = "k7/8/8/6p1/7P/8/8/K7 b - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position101) {
  std::string fen = "k7/8/6p1/8/8/7P/8/K7 b - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position102) {
  std::string fen = "k7/8/8/3p4/4p3/8/8/7K b - - 0 1";
  std::vector<int> expected = {5, 15, 102, 569, 4337, 22579};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position103) {
  std::string fen = "k7/8/3p4/8/8/4P3/8/7K b - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4271, 28662};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position104) {
  std::string fen = "7k/8/8/p7/1P6/8/8/7K w - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position105) {
  std::string fen = "7k/8/p7/8/8/1P6/8/7K w - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position106) {
  std::string fen = "7k/8/8/1p6/P7/8/8/7K w - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position107) {
  std::string fen = "7k/8/1p6/8/8/P7/8/7K w - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position108) {
  std::string fen = "k7/7p/8/8/8/8/6P1/K7 w - - 0 1";
  std::vector<int> expected = {5, 25, 161, 1035, 7574, 55338};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position109) {
  std::string fen = "k7/6p1/8/8/8/8/7P/K7 w - - 0 1";
  std::vector<int> expected = {5, 25, 161, 1035, 7574, 55338};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position110) {
  std::string fen = "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1";
  std::vector<int> expected = {7, 49, 378, 2902, 24122, 199002};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position111) {
  std::string fen = "7k/8/8/p7/1P6/8/8/7K b - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position112) {
  std::string fen = "7k/8/p7/8/8/1P6/8/7K b - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position113) {
  std::string fen = "7k/8/8/1p6/P7/8/8/7K b - - 0 1";
  std::vector<int> expected = {5, 22, 139, 877, 6112, 41874};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position114) {
  std::string fen = "7k/8/1p6/8/8/P7/8/7K b - - 0 1";
  std::vector<int> expected = {4, 16, 101, 637, 4354, 29679};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position115) {
  std::string fen = "k7/7p/8/8/8/8/6P1/K7 b - - 0 1";
  std::vector<int> expected = {5, 25, 161, 1035, 7574, 55338};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position116) {
  std::string fen = "k7/6p1/8/8/8/8/7P/K7 b - - 0 1";
  std::vector<int> expected = {5, 25, 161, 1035, 7574, 55338};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position117) {
  std::string fen = "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1";
  std::vector<int> expected = {7, 49, 378, 2902, 24122, 199002};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position118) {
  std::string fen = "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1";
  std::vector<int> expected = {11, 97, 887, 8048, 90606};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position119) {
  std::string fen = "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1";
  std::vector<int> expected = {24, 421, 7421, 124608};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position120) {
  std::string fen = "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1";
  std::vector<int> expected = {18, 270, 4699, 79355};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position121) {
  std::string fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1";
  std::vector<int> expected = {24, 496, 9483, 182838};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position122) {
  std::string fen = "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1";
  std::vector<int> expected = {11, 97, 887, 8048, 90606};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position123) {
  std::string fen = "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1";
  std::vector<int> expected = {24, 421, 7421, 124608};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position124) {
  std::string fen = "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1";
  std::vector<int> expected = {18, 270, 4699, 79355};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position125) {
  std::string fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";
  std::vector<int> expected = {24, 496, 9483, 182838};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position126) {
  std::string fen = "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1";
  std::vector<int> expected = {15, 126, 1928, 13931, 206379, 1440467};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position127) {
  std::string fen = "8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1";
  std::vector<int> expected = {15, 126, 1928, 13931, 206379, 1440467};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position128) {
  std::string fen = "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1";
  std::vector<int> expected = {18, 92, 1670, 10138, 185429, 1134888};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position129) {
  std::string fen = "8/8/8/8/k1p4R/8/3P4/3K4 w - - 0 1";
  std::vector<int> expected = {18, 92, 1670, 10138, 185429, 1134888};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position130) {
  std::string fen = "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1";
  std::vector<int> expected = {13, 102, 1266, 10276, 135655, 1015133};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position131) {
  std::string fen = "8/b2p2k1/8/2P5/8/4K3/8/8 b - - 0 1";
  std::vector<int> expected = {13, 102, 1266, 10276, 135655, 1015133};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position132) {
  std::string fen = "5k2/8/8/8/8/8/8/4K2R w K - 0 1";
  std::vector<int> expected = {15, 66, 1198, 6399, 120330, 661072};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position133) {
  std::string fen = "4k2r/8/8/8/8/8/8/5K2 b k - 0 1";
  std::vector<int> expected = {15, 66, 1198, 6399, 120330, 661072};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position134) {
  std::string fen = "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1";
  std::vector<int> expected = {16, 71, 1286, 7418, 141077, 803711};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position135) {
  std::string fen = "r3k3/8/8/8/8/8/8/3K4 b q - 0 1";
  std::vector<int> expected = {16, 71, 1286, 7418, 141077, 803711};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position136) {
  std::string fen = "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1";
  std::vector<int> expected = {26, 1141, 27826, 1274206};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position137) {
  std::string fen = "r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1";
  std::vector<int> expected = {26, 1141, 27826, 1274206};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position138) {
  std::string fen = "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1";
  std::vector<int> expected = {44, 1494, 50509, 1720476};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position139) {
  std::string fen = "r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1";
  std::vector<int> expected = {44, 1494, 50509, 1720476};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position140) {
  std::string fen = "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1";
  std::vector<int> expected = {11, 133, 1442, 19174, 266199, 3821001};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position141) {
  std::string fen = "3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1";
  std::vector<int> expected = {11, 133, 1442, 19174, 266199, 3821001};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position142) {
  std::string fen = "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1";
  std::vector<int> expected = {29, 165, 5160, 31961, 1004658};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position143) {
  std::string fen = "5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1";
  std::vector<int> expected = {29, 165, 5160, 31961, 1004658};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position144) {
  std::string fen = "4k3/1P6/8/8/8/8/K7/8 w - - 0 1";
  std::vector<int> expected = {9, 40, 472, 2661, 38983, 217342};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position145) {
  std::string fen = "8/k7/8/8/8/8/1p6/4K3 b - - 0 1";
  std::vector<int> expected = {9, 40, 472, 2661, 38983, 217342};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position146) {
  std::string fen = "8/P1k5/K7/8/8/8/8/8 w - - 0 1";
  std::vector<int> expected = {6, 27, 273, 1329, 18135, 92683};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position147) {
  std::string fen = "8/8/8/8/8/k7/p1K5/8 b - - 0 1";
  std::vector<int> expected = {6, 27, 273, 1329, 18135, 92683};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position148) {
  std::string fen = "K1k5/8/P7/8/8/8/8/8 w - - 0 1";
  std::vector<int> expected = {2, 6, 13, 63, 382, 2217};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position149) {
  std::string fen = "8/8/8/8/8/p7/8/k1K5 b - - 0 1";
  std::vector<int> expected = {2, 6, 13, 63, 382, 2217};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position150) {
  std::string fen = "8/k1P5/8/1K6/8/8/8/8 w - - 0 1";
  std::vector<int> expected = {10, 25, 268, 926, 10857, 43261, 567584};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position151) {
  std::string fen = "8/8/8/8/1k6/8/K1p5/8 b - - 0 1";
  std::vector<int> expected = {10, 25, 268, 926, 10857, 43261, 567584};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position152) {
  std::string fen = "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1";
  std::vector<int> expected = {37, 183, 6559, 23527};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position153) {
  std::string fen = "1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1";
  std::vector<int> expected = {13, 284, 3529, 85765, 1063513};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position154) {
  std::string fen = "4k2r/8/8/7r/8/8/1B6/1K6 w k - 0 1";
  std::vector<int> expected = {13, 284, 3529, 85765, 1063513};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position155) {
  std::string fen = "1k6/8/8/8/R7/1n6/8/R3K3 b Q - 0 1";
  std::vector<int> expected = {9, 193, 1676, 38751, 346695};
  test_fen(fen, expected);
}
TEST(MoveGenTests, Position156) {
  std::string fen = "r3k3/8/1N6/r7/8/8/8/1K6 w q - 0 1";
  std::vector<int> expected = {9, 193, 1676, 38751, 346695};
  test_fen(fen, expected);
}
