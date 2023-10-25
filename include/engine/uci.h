#ifndef UCI_H
#define UCI_H

#include <string>

#include "board.h"
#include "board_helper.h"
#include "engine/engine.h"


class UCI {
    public:
        UCI(Board& board);

        void process_command(const std::string& command);

    private:
        Board& board;
        BoardHelper board_helper;
        Engine engine;

        void handle_uci_command();
        void handle_isready_command();
        void handle_ucinewgame_command();
        void make_moves(std::istringstream& moves);
        void handle_position_command(const std::string& position);
        void handle_go_perft_command(const std::string& depth_argument);
        void handle_go_command();
};

#endif
