#ifndef UCI_HPP
#define UCI_HPP

#include <sstream>
#include <string>
#include <vector>

#include "board.hpp"
#include "move_gen.hpp"
#include "search.hpp"


class UCI {
    public:
        UCI(Board& board);

        void process(const std::string& input);
        static void show(const SearchSummary& search_summary);
        static void bestmove(const Move& move);

    private:
        Board& board;
        MoveGenerator move_gen;
        Search search;

        static const int MOVE_OVERHEAD = 50;

        static void uci();
        static void isready();
        void position(std::istringstream& arguments);
        SearchParams get_search_params(const std::vector<std::string>& arguments) const;

        void make_moves(std::istringstream& moves);
        bool make_move(const std::string& move);
};

#endif
