#ifndef PSQT_H
#define PSQT_H

#include <array>


extern std::array<int, 64> PAWN_PSQT; 
extern std::array<int, 64> KNIGHT_PSQT; 
extern std::array<int, 64> BISHOP_PSQT; 
extern std::array<int, 64> ROOK_PSQT; 
extern std::array<int, 64> QUEEN_PSQT; 
extern std::array<int, 64> KING_PSQT; 
extern std::array<int, 64> KING_ENDGAME_PSQT; 

// used when there is only the king left
// in order to force the king to the edge of the board
// and eventually mate
extern std::array<int, 64> KING_MATE; 

#endif
