#pragma once

#include "board.hpp"

int perft(std::unique_ptr<Board> &board, int depth);
void divide(std::unique_ptr<Board> &board, int depth);
