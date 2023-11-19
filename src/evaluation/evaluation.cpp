#include "evaluation.hpp"

#include "board.hpp"
#include "piece.hpp"


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

int Piece::get_value() const {
    switch (piece_type) {
        case KING: return KING_VALUE;
        case QUEEN: return QUEEN_VALUE;
        case ROOK: return ROOK_VALUE;
        case BISHOP: return BISHOP_VALUE;
        case KNIGHT: return KNIGHT_VALUE;
        case PAWN: return PAWN_VALUE;
    }
}

int Board::get_psqt_score(const Piece& piece) const {
    const int x = piece.pos % 8;
    const int y = piece.pos / 8;
    const int square = piece.color == WHITE ? piece.pos : x + (7 - y) * 8;
    switch (piece.piece_type) {
        case KING: {
            if (is_lone_king(piece.color)) {
                return KING_MATE[square];
            }
            if (is_endgame()) {
                return KING_ENDGAME_PSQT[square];
            }
            return KING_PSQT[square];
        }
        case QUEEN: return QUEEN_PSQT[square];
        case ROOK: return ROOK_PSQT[square];
        case BISHOP: return BISHOP_PSQT[square];
        case KNIGHT: return KNIGHT_PSQT[square];
        case PAWN: return PAWN_PSQT[square];
    }
}

bool Board::is_lone_king(Color color) const {
    return pieces[color].size() == 1;
}

bool Board::is_endgame() const {
    return game_state.material[WHITE] - KING_VALUE < 1500 && 
           game_state.material[BLACK] - KING_VALUE < 1500;
}
