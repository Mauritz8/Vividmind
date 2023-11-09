#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "board.hpp"
#include "move_gen.hpp"
#include "test_positions.hpp"

struct Test {
    std::string fen;
    int depth;
    int expected_nodes;
    int actual_nodes;
};

int main() {
    Board board;
    MoveGenerator move_gen(board);

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
            
            if (delim_index == std::string::npos) break;
        }

        const std::string fen = tokens.at(0);
        board = board.get_position_from_fen(fen);
        std::cout << "Test " << i + 1 << "\n";
        std::cout << "FEN: " << fen <<"\n";
        for (int i = 1; i < tokens.size(); i++) {
            std::string token = tokens.at(i);
            const size_t split_index = token.find(' ');

            std::string depth_str = token.substr(0, split_index); 
            const int depth = std::stoi(depth_str.substr(1));
            const int expected_nodes = std::stoi(token.substr(split_index)); 

            const auto start = std::chrono::high_resolution_clock::now();
            const int actual_nodes = move_gen.perft(depth);
            const auto finished = std::chrono::high_resolution_clock::now();
            const int duration = std::chrono::duration_cast<std::chrono::milliseconds>(finished - start).count();
            total_duration += duration;
            const bool is_successful = actual_nodes == expected_nodes;
            std::cout << "depth " << depth << ": found ";
            std::cout << actual_nodes << "/" << expected_nodes << " moves"; 
            std::cout << " - " << (is_successful ? "[OK]" : "[FAIL]");
            std::cout << " (" << "" << duration << " ms)" << "\n";

            if (!is_successful) {
                failed_tests.push_back(Test{fen, depth, expected_nodes, actual_nodes});
            }
        }
        std::cout << "\n";
    }

    const int nr_failed_tests = failed_tests.size();
    std::cout << "\n\nFinished in " << total_duration / 1000 << " s\n";
    std::cout << nr_tests - nr_failed_tests << "/" << nr_tests << " tests were successful\n\n";
    if (nr_failed_tests > 0) {
        std::cout << "Failed tests:\n";
        for (Test test : failed_tests) {
            std::cout << "FEN: " << test.fen << "(depth " << test.depth << " found ";
            std::cout << test.actual_nodes << "/" << test.expected_nodes << " moves)\n"; 
        }
        return 1;
    }
    return 0;
}
