#ifndef UCI_H
#define UCI_H

#include <string>

#include "board.hpp"
#include "engine/engine.hpp"


class UCI {
    public:
        UCI(Board& board);

        void process(const std::string& command);

    private:
        Board& board;
        MoveGenerator move_gen;
        Engine engine;

        static const int MOVE_OVERHEAD = 50;

        void uci();
        void isready();
        void position(const std::string& position);
        void go(const std::string& arguments);

        void make_moves(std::istringstream& moves);
        bool make_move(const std::string& move);
};

#endif
