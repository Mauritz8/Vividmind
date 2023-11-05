#include "engine/uci.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "move.hpp"
#include "move_generator.hpp"


UCI::UCI(Board& board) 
    : board(board)
    , engine(board)
{}

void UCI::process(const std::string& command) {
    if (command == "uci") {
        uci();
    } else if (command == "isready") {
        isready();
    } else if (command.substr(0, 8) == "position") {
        position(command.substr(9, std::string::npos));
    } else if (command.substr(0, 2) == "go") {
        go(command.substr(3, std::string::npos));
    } else if (command == "quit") {
        exit(0);
    }
    std::cout.flush();
}

void UCI::uci() {
    std::cout << "id name Vividmind\n";
    std::cout << "id author Mauritz Sjödin\n"; 
    std::cout << "uciok\n\n";
}

void UCI::isready() {
    std::cout << "readyok\n\n";
}

void UCI::position(const std::string& position) {
    std::istringstream ss(position);
    std::string token;

    std::getline(ss, token, ' ');
    if (token == "startpos") {
        board = Board::get_starting_position();
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

void UCI::go(const std::string& arguments) {
    std::istringstream ss(arguments);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        std::string argument;
        std::getline(ss, argument, ' ');

        if (token == "wtime") {
            engine.search_params.wtime = std::stoi(argument);
        } else if (token == "btime") {
            engine.search_params.btime = std::stoi(argument);
        } else if (token == "winc") {
            engine.search_params.winc = std::stoi(argument);
        } else if (token == "binc") {
            engine.search_params.binc = std::stoi(argument);
        } else if (token == "movestogo") {
            engine.search_params.moves_to_go = std::stoi(argument);
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


void UCI::make_moves(std::istringstream& moves) {
    std::string move_uci;
    while (std::getline(moves, move_uci, ' ')) {
        const bool is_legal = make_move(move_uci);
        if (!is_legal) {
            return;
        }
    }
}

bool UCI::make_move(const std::string& move_uci) {
    MoveGenerator move_gen(board);
    const Color player = board.game_state.player_to_move;
    const std::vector<Move> pseudo_legal_moves = move_gen.get_pseudo_legal_moves(ALL);
    for (const Move& move : pseudo_legal_moves) {
        if (move.to_uci_notation() == move_uci) {
            board.make(move);
            if (move_gen.is_in_check(player)) {
                board.undo();
                return false;
            }
            return true;
        } 
    }
    return false;
}
