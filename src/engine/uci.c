#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void handle_ucinewgame_command(Board* board, MoveArray* move_history) {
    deallocate_game_resources(board, move_history);

    setup_board(board); 
    *move_history = create_empty_move_history();
}

static void handle_position_command(char* position, Board* board, MoveArray* move_history) {
    char* token = strtok(position, " ");
    if (strcmp(token, "startpos") == 0) {
        handle_ucinewgame_command(board, move_history);
    }

    while ((token = strtok(NULL, " ")) != NULL) {
        if (strcmp(token, "moves") == 0) {
            while ((token = strtok(NULL, " ")) != NULL) {
                Move move = uci_notation_to_move(token, board);
                make_appropriate_move(&move, board, move_history);
            }
        }
    }
}

static void handle_go_command(Board* board, MoveArray* move_history) {
    const int depth = 1;
    Move best_move = get_best_move(depth, board, move_history);
    printf("bestmove %s\n", move_to_uci_notation(&best_move));
}

void process_uci_command(char* command, Board* board, MoveArray* move_history) {
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
