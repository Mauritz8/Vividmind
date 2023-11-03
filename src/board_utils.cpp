#include "board_utils.hpp"


Color get_opposite_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}
