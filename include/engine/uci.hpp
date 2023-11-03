#ifndef UCI_H
#define UCI_H

#include <string>

#include "board.hpp"
#include "engine/engine.hpp"


class UCI {
    public:
        UCI(Board& board);

        void process_command(const std::string& command);

    private:
        Board& board;
        Engine engine;

        void handle_uci_command();
        void handle_isready_command();
        void handle_ucinewgame_command();
        void make_moves(std::istringstream& moves);
        void handle_position_command(const std::string& position);
        void handle_go_command(const std::string& arguments);
};

#endif
