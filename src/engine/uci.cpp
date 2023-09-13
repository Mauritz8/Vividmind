#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "board.h"
#include "engine/uci.h"
#include "engine/engine.h"
#include "game_state.h"
#include "move.h"

static void handle_uci_command() {
    puts("id name Stockfish 2.0");
    puts("id author DemonicBag");
    puts("uciok\n");
}

static void handle_isready_command() {
    puts("readyok\n");
}

static void handle_ucinewgame_command(Board& board, std::vector<Move>& move_history) {
    board = Board::get_starting_position();
    move_history = {};
}

static void handle_position_command(char* position, Board& board, std::vector<Move> move_history) {
    char* token = strtok(position, " ");
    if (strcmp(token, "startpos") == 0) {
        handle_ucinewgame_command(board, move_history);
    }

    while ((token = strtok(NULL, " ")) != NULL) {
        if (strcmp(token, "moves") == 0) {
            while ((token = strtok(NULL, " ")) != NULL) {
                Move move = Move(token, board);
                if (move.is_legal_move(board, move_history)) {
                    move.make_appropriate_move(board, move_history);
                }
            }
        }
    }
}

static void handle_go_command(Board& board, std::vector<Move> move_history) {
    const int depth = 1;
    Move best_move = get_best_move(depth, board, move_history);
    std::cout << "bestmove " << best_move.move_to_uci_notation() << "\n";
}

void process_uci_command(char* command, Board& board, std::vector<Move> move_history) {
    if (strcmp(command, "uci") == 0) {
        handle_uci_command();
    } else if (strcmp(command, "isready") == 0) {
        handle_isready_command();
    } else if (strcmp(command, "ucinewgame") == 0) {
        handle_ucinewgame_command(board, move_history);
    } else if (strncmp(command, "position", 8) == 0) {
        handle_position_command(command + 9, board, move_history);
    } else if (strncmp(command, "go", 2) == 0) {
        handle_go_command(board, move_history);
    } else if (strcmp(command, "quit") == 0) {
        exit(0);
    }
    fflush(stdout);
}
