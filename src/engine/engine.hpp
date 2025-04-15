#pragma once

#include <atomic>
#include <memory>

#include "board.hpp"
#include "engine/command.hpp"

namespace engine {
void execute_command(Command command, std::atomic<bool> &stop,
                     std::unique_ptr<Board> &board);
};
