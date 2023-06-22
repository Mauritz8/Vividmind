#include <stdlib.h>

#include "square.h"

void square_array_push(SquareArray* square_array, Square* square) {
    if (square_array->length == square_array->capacity) {
        square_array->capacity *= 2;
        square_array->squares = realloc(square_array->squares, square_array->capacity * sizeof(Square));
    }
    square_array->squares[square_array->length++] = square;
}
