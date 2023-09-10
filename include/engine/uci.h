#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "move.h"
#include <vector>

void process_uci_command(char* command, Board& board, std::vector<Move> move_history);

#endif
