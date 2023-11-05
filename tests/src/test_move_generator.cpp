#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "board.hpp"
#include "move_generator.hpp"
#include "test_positions.cpp"

class MoveGenerationTest : public testing::Test {
    public:
        Board board;
        MoveGenerator move_gen;

        MoveGenerationTest() 
            : board(Board())
            , move_gen(board)
        {}
};

TEST_F(MoveGenerationTest, MoveGenerationTestSuite) {
    for (int i = 0; i < test_positions.size(); i++) {
        std::string position = test_positions.at(i);
        size_t pos;
        std::vector<std::string> tokens;
        while (true) {
            pos = position.find(';');
            tokens.push_back(position.substr(0, pos)); 
            position.erase(0, pos + 1);
            
            if (pos == std::string::npos) break;
        }

        const std::string fen = tokens.at(0);
        board = board.get_position_from_fen(fen);
        std::cout << "Test " << i + 1 << "\n";
        std::cout << "FEN: " << fen <<"\n";
        for (int i = 1; i < tokens.size(); i++) {
            std::string token = tokens.at(i);
            const size_t split_index = token.find(' ');

            std::string depth_str = token.substr(0, split_index); 
            const int depth = std::stoi(depth_str.substr(1, std::string::npos));

            token.erase(0, split_index + 1);
            const int expected_nodes = std::stoi(token) + 1; 


            const int actual_nodes = move_gen.perft(depth);
            EXPECT_EQ(actual_nodes, expected_nodes);      
            if (actual_nodes == expected_nodes) {
                std::cout << "depth " << depth 
                << ": found " << actual_nodes 
                << "/" << expected_nodes << " positions\n"; 
            }
        }
        std::cout << "\n";
    }
}
