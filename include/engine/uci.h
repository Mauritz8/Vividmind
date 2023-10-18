#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "move.h"
#include <string>
#include <vector>

void process_uci_command(const std::string& command, Board& board);

#endif
