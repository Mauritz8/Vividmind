#ifndef GAME_OVER_CONDITIONS_H
#define GAME_OVER_CONDITIONS_H

#include "board.h"

bool is_checkmated(const Color color, Board* board);
bool is_insufficient_material(Board* board);

#endif
