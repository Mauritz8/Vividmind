#include "pos.hpp"


bool operator==(Pos pos1, Pos pos2) {
    return pos1.x == pos2.x && pos1.y == pos2.y;
}

bool is_outside_board(Pos pos) {
    return pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7;
}
