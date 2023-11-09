#ifndef UCI_HPP
#define UCI_HPP

#include <sstream>
#include <string>

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
        void go(std::istringstream& arguments);

        void make_moves(std::istringstream& moves);
        bool make_move(const std::string& move);
};

#endif
