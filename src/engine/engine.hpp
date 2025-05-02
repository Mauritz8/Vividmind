#pragma once

#include <atomic>

#include "board/board.hpp"
#include "engine/command.hpp"

namespace engine {
void execute_command(const Command &command, std::atomic<bool> &stop,
                     Board &board);
};
