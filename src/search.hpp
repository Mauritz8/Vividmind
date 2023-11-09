#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <vector>

#include "board.hpp"
#include "move_gen.hpp"
#include "search/defs.hpp"


class Search {
    public:
        SearchParams params;

        Search(Board& board, MoveGenerator& move_gen);

        void iterative_deepening_search();
        int calc_allocated_time();

    private:
        Board& board;
        MoveGenerator& move_gen;

        SearchInfo info;

        int search(int depth, int alpha, int beta, std::vector<Move>& principal_variation);
        int search_captures(int alpha, int beta);
        void check_termination();
};

#endif