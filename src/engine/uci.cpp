#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "board.h"
#include "engine/uci.h"
#include "engine/engine.h"
#include "game_state.h"
#include "move.h"


static void handle_uci_command() {
    std::cout << "id name Vividmind\n";
    std::cout << "id author Mauritz Sjödin\n"; 
    std::cout << "uciok\n\n";
}

static void handle_isready_command() {
    std::cout << "readyok\n\n";
}

static void handle_ucinewgame_command(Board& board) {
    board = Board::get_starting_position();
}

static void make_moves(std::istringstream& moves, Board& board) {
    std::string move_uci;
    while (std::getline(moves, move_uci, ' ')) {
        Move move = Move::get_from_uci_notation(move_uci, board);
        move.make_appropriate(board);
    }
}

static void handle_position_command(const std::string& position, Board& board) {
    std::istringstream ss(position);
    std::string token;

    std::getline(ss, token, ' ');
    if (token == "startpos") {
        handle_ucinewgame_command(board);
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
        make_moves(ss, board);
    }
}

static void handle_go_perft_command(const std::string& depth_argument, Board& board) {
    try {
        const int depth = std::stoi(depth_argument);
        divide(depth, board);
    } catch (const std::invalid_argument& e) {}
}

static void handle_go_command(Board& board) {
    const int depth = 3;
    const Move best_move = get_best_move(depth, board);
    std::cout << "bestmove " << best_move.to_uci_notation() << "\n";
}

void process_uci_command(const std::string& command, Board& board) {
    if (command == "uci") {
        handle_uci_command();
    } else if (command == "isready") {
        handle_isready_command();
    } else if (command == "ucinewgame") {
        handle_ucinewgame_command(board);
    } else if (command.substr(0, 8) == "position") {
        handle_position_command(command.substr(9, std::string::npos), board);
    } else if (command.substr(0, 8) == "go perft") {
        handle_go_perft_command(command.substr(9, std::string::npos), board);
    } else if (command.substr(0, 2) == "go") {
        handle_go_command(board);
    } else if (command == "quit") {
        exit(0);
    }
    fflush(stdout);
}
