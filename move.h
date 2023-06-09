#ifndef MOVE_H
#define MOVE_H

#include "square.h"

struct Move {
    Square* start_square;
    Square* end_square;
};

#endif
