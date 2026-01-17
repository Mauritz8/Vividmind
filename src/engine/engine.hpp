#pragma once

#include <atomic>

#include "board/board.hpp"
#include "engine/command.hpp"

const int MAX_DEPTH = 100;
const int MAX_TIME = 3600000;

namespace engine {
void execute_command(const Command &command, std::atomic<bool> &stop,
                     Board &board);
};
