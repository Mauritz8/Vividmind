#include "uci.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "move.hpp"
#include "move_generator.hpp"


UCI::UCI(Board& board) 
    : board(board)
    , move_gen(board)
    , engine(board, move_gen)
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

void UCI::show(const SearchSummary& search_summary) {
    std::cout << "info";
    std::cout << " depth " << search_summary.depth;
    if (std::abs(search_summary.score) > CHECKMATE_THRESHOLD) {
        const int ply = CHECKMATE - std::abs(search_summary.score);
        const int mate_in_x = std::ceil(ply / 2.0);
        const int sign = search_summary.score > 0 ? 1 : -1;
        std::cout << " score mate " << sign * mate_in_x;
    } else {
        std::cout << " score cp " << search_summary.score;
    }
    std::cout << " nodes " << search_summary.nodes;
    std::cout << " nps " << (search_summary.time == 0 
        ? search_summary.nodes * 1000 / 1
        : search_summary.nodes * 1000 / search_summary.time);
    std::cout << " time " << search_summary.time;
    std::cout << " pv";
    for (const Move& move : search_summary.pv) {
        std::cout << " " << move.to_uci_notation();
    }
    std::cout << "\n";
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
    engine.search_params = SearchParams();

    std::istringstream ss(arguments);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        std::string argument;
        std::getline(ss, argument, ' ');

        if (token == "wtime") {
            engine.search_params.game_time.wtime = std::stoi(argument);
        } else if (token == "btime") {
            engine.search_params.game_time.btime = std::stoi(argument);
        } else if (token == "winc") {
            engine.search_params.game_time.winc = std::stoi(argument);
        } else if (token == "binc") {
            engine.search_params.game_time.binc = std::stoi(argument);
        } else if (token == "movestogo") {
            engine.search_params.game_time.moves_to_go = std::stoi(argument);
        }
        

        else if (token == "perft") {
            int depth =  std::stoi(argument);
            move_gen.divide(depth);
            return;
        }

        else if (token == "depth") {
            int depth =  std::stoi(argument);
            engine.search_params.search_mode = DEPTH;
            engine.search_params.depth = depth;
        } else if (token == "movetime") {
            int movetime =  std::stoi(argument);
            engine.search_params.search_mode = MOVE_TIME;
            engine.search_params.allocated_time = movetime - MOVE_OVERHEAD;
        }
    }
    
    if (engine.search_params.game_time.wtime != 0 &&
        engine.search_params.game_time.btime != 0) 
    {
        engine.search_params.search_mode = MOVE_TIME;
        engine.search_params.allocated_time = engine.get_allocated_time() - MOVE_OVERHEAD;
    }

    engine.iterative_deepening_search();
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
