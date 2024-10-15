#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "board.hpp"
#include "board/fen.hpp"
#include "fmt/core.h"
#include "perft.hpp"
#include "test_positions.hpp"

struct Test {
  std::string fen;
  int depth;
  int expected_nodes;
  int actual_nodes;
};

int main() {
  int total_duration = 0;
  std::vector<Test> failed_tests;
  const int nr_tests = test_positions.size();
  for (int i = 0; i < test_positions.size(); i++) {
    std::string position = test_positions.at(i);
    size_t delim_index;
    std::vector<std::string> tokens;
    while (true) {
      delim_index = position.find(';');
      tokens.push_back(position.substr(0, delim_index));
      position.erase(0, delim_index + 1);

      if (delim_index == std::string::npos)
        break;
    }

    const std::string fen = tokens.at(0);
    Board board = fen::get_position(fen);
    fmt::println("Test {}", i + 1);
    fmt::println("FEN: {}", fen);
    for (int i = 1; i < tokens.size(); i++) {
      std::string token = tokens.at(i);
      const size_t split_index = token.find(' ');

      std::string depth_str = token.substr(0, split_index);
      const int depth = std::stoi(depth_str.substr(1));
      const int expected_nodes = std::stoi(token.substr(split_index));

      const auto start = std::chrono::high_resolution_clock::now();
      const int actual_nodes = perft(board, depth);
      const auto finished = std::chrono::high_resolution_clock::now();
      const int duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(finished -
                                                                start)
              .count();
      total_duration += duration;
      const bool is_successful = actual_nodes == expected_nodes;
      fmt::println("depth {}: found {}/{} moves - {} ({} ms)", depth,
                   actual_nodes, expected_nodes,
                   is_successful ? "[OK]" : "[FAIL]", duration);
      if (!is_successful) {
        failed_tests.push_back(Test{fen, depth, expected_nodes, actual_nodes});
      }
    }
    fmt::print("\n");
  }

  const int nr_failed_tests = failed_tests.size();
  fmt::println("\n\nFinished in {} s", total_duration / 1000);
  fmt::println("{}/{} tests were successful\n", nr_tests - nr_failed_tests,
               nr_tests);
  if (nr_failed_tests > 0) {
    fmt::println("Failed tests:");
    for (Test test : failed_tests) {
      fmt::println("FEN: {} (depth {} found {}/{} moves)", test.fen, test.depth,
                   test.actual_nodes, test.expected_nodes);
    }
    return 1;
  }
  return 0;
}
