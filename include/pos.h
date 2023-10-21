#ifndef POS_H
#define POS_H


struct Pos {
    int x;
    int y;
};
bool operator==(Pos pos1, Pos pos2);
bool is_outside_board(Pos pos);

#endif
