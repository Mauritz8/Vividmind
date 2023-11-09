#include "search.hpp"


// the evaluation function is very straight forward
// it only considers material and piece-square tables
int Board::evaluate() const {

    // the evaluation from white's point of view
    int evaluation = 0;

    // calculate the material balance from white's point of view
    const int white_material = game_state.material[WHITE];
    const int black_material = game_state.material[BLACK];
    evaluation += white_material - black_material;

    // calculate the difference between the piece-square table values 
    // also from white's point of view
    const int white_psqt = game_state.psqt[WHITE];
    const int black_psqt = game_state.psqt[BLACK];
    evaluation += white_psqt - black_psqt;

    // always return the score from the view of the player to move
    // so a positive value always means a good position
    // and a negative value means a bad position
    if (game_state.player_to_move == BLACK) {
        return -evaluation;
    }
    return evaluation;
}