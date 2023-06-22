#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "move.h"

void process_uci_command(char* command, Board* board, MoveArray* move_history);

#endif
