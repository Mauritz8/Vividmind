#pragma once

#include <atomic>

#include "bitboards_board/bitboards_board.hpp"
#include "engine/command.hpp"

namespace engine {
void execute_command(const Command &command, std::atomic<bool> &stop,
                     BitboardsBoard &board);
};
