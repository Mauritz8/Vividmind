#ifndef UCI_HPP
#define UCI_HPP

#include <string>

#include "board.hpp"
#include "search.hpp"


class UCI {
    public:
        UCI(Board& board);

        void process(const std::string& command);
        static void show(const SearchSummary& search_summary);

    private:
        Board& board;
        MoveGenerator move_gen;
        Search search;

        static const int MOVE_OVERHEAD = 50;

        void uci();
        void isready();
        void position(const std::string& position);
        void go(const std::string& arguments);

        void make_moves(std::istringstream& moves);
        bool make_move(const std::string& move);
};

#endif
