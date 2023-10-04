#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "board.h"
#include "engine/uci.h"
#include "engine/engine.h"
#include "game_state.h"
#include "move.h"

static void handle_uci_command() {
    std::cout << "id name Stockfish 2.0\n";
    std::cout << "id author DemonicBag\n"; 
    std::cout << "uciok\n\n";
}

static void handle_isready_command() {
    std::cout << "readyok\n\n";
}

static void handle_ucinewgame_command(Board& board, std::vector<Move>& move_history) {
    board = Board::get_starting_position();
    move_history = {};
}

static void make_moves(std::istringstream& moves, Board& board, std::vector<Move>& move_history) {
    std::string move_uci;
    while (std::getline(moves, move_uci, ' ')) {
        Move move = Move::get_from_uci_notation(move_uci, board);
        move.make_appropriate(board, move_history);
    }
}

static void handle_position_command(const std::string& position, Board& board, std::vector<Move>& move_history) {
    std::istringstream ss(position);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        if (token == "startpos") {
            handle_ucinewgame_command(board, move_history);
        } else if (token == "moves") {
            make_moves(ss, board, move_history);
            break;
        }
    }
}

static void handle_go_command(Board& board, std::vector<Move>& move_history) {
    const int depth = 1;
    const Move best_move = get_best_move(depth, board, move_history);
    std::cout << "bestmove " << best_move.to_uci_notation() << "\n";
}

void process_uci_command(const std::string& command, Board& board, std::vector<Move>& move_history) {
    if (command == "uci") {
        handle_uci_command();
    } else if (command == "isready") {
        handle_isready_command();
    } else if (command == "ucinewgame") {
        handle_ucinewgame_command(board, move_history);
    } else if (command.substr(0, 8) == "position") {
        handle_position_command(command.substr(9, std::string::npos), board, move_history);
    } else if (command.substr(0, 2) == "go") {
        handle_go_command(board, move_history);
    } else if (command == "quit") {
        exit(0);
    }
    fflush(stdout);
}
