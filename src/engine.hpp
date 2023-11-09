#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "board.hpp"
#include "move_generator.hpp"
#include "search/defs.hpp"


class Engine {
    public:
        SearchParams search_params;

        Engine(Board& board, MoveGenerator& move_gen);

        void iterative_deepening_search();
        int get_allocated_time();

    private:
        Board& board;
        MoveGenerator& move_gen;

        SearchInfo search_info;

        int search(int depth, int alpha, int beta, std::vector<Move>& principal_variation);
        int search_captures(int alpha, int beta);
        int evaluate();
        void check_termination();
};


#endif
