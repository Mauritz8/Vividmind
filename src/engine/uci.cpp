#include "engine/uci.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "board_helper.hpp"
#include "move.hpp"


UCI::UCI(Board& board) 
    : board(board)
    , board_helper(board)
    , engine(board, board_helper)
{}

void UCI::process_command(const std::string& command) {
    if (command == "uci") {
        handle_uci_command();
    } else if (command == "isready") {
        handle_isready_command();
    } else if (command == "ucinewgame") {
        handle_ucinewgame_command();
    } else if (command.substr(0, 8) == "position") {
        handle_position_command(command.substr(9, std::string::npos));
    } else if (command.substr(0, 2) == "go") {
        handle_go_command(command.substr(3, std::string::npos));
    } else if (command == "quit") {
        exit(0);
    }
    std::cout.flush();
}

void UCI::handle_uci_command() {
    std::cout << "id name Vividmind\n";
    std::cout << "id author Mauritz Sjödin\n"; 
    std::cout << "uciok\n\n";
}

void UCI::handle_isready_command() {
    std::cout << "readyok\n\n";
}

void UCI::handle_ucinewgame_command() {
    board = Board::get_starting_position();
}

void UCI::make_moves(std::istringstream& moves) {
    std::string move_uci;
    while (std::getline(moves, move_uci, ' ')) {
        Move move = Move::get_from_uci_notation(move_uci, board);
        board_helper.make_appropriate(move);
    }
}

void UCI::handle_position_command(const std::string& position) {
    std::istringstream ss(position);
    std::string token;

    std::getline(ss, token, ' ');
    if (token == "startpos") {
        handle_ucinewgame_command();
    } else if (token == "fen") {
        std::string fen;
        std::string fen_part;
        for (int i = 0; i < 6; i++) {
            std::getline(ss, fen_part, ' ');
            fen += fen_part + ' ';
        }
        board = Board::get_position_from_fen(fen);
    } 

    std::getline(ss, token, ' ');
    if (token == "moves") {
        make_moves(ss);
    }
}

void UCI::handle_go_command(const std::string& arguments) {
    std::istringstream ss(arguments);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        std::string argument;
        std::getline(ss, argument, ' ');

        if (token == "wtime") {
            engine.wtime = std::stoi(argument);
        } else if (token == "btime") {
            engine.btime = std::stoi(argument);
        } else if (token == "winc") {
            engine.btime = std::stoi(argument);
        } else if (token == "binc") {
            engine.btime = std::stoi(argument);
        } else if (token == "movestogo") {
            engine.moves_to_go = std::stoi(argument);
        }
        

        else if (token == "perft") {
            int depth =  std::stoi(argument);
            engine.divide(depth);
            return;
        }

        else if (token == "depth") {
            int depth =  std::stoi(argument);
            engine.iterative_deepening_search_depth(depth);
            return;
        } else if (token == "movetime") {
            int movetime =  std::stoi(argument);
            engine.iterative_deepening_search_time(movetime);
            return;
        } else if (token == "infinite") {
            engine.iterative_deepening_search_infinite(); 
            return;
        }
    }
    int allocated_time = engine.get_allocated_time();
    engine.iterative_deepening_search_time(allocated_time);
}
