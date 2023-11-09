#include "board.hpp"

#include <array>
#include <iostream>
#include <vector>


bool Board::operator==(const Board& other) const {
    for (int pos = 0; pos < 64; pos++) {
        if (squares[pos].piece != other.squares[pos].piece) {
            return false;
        } 
    }

    if (game_state.en_passant_square != other.game_state.en_passant_square) {
        return false;
    }

    std::array<Color, 2> colors = {WHITE, BLACK};
    for (Color color : colors) {
        if (game_state.castling_rights[color].kingside != other.game_state.castling_rights[color].kingside) {
            return false;
        }
        if (game_state.castling_rights[color].queenside != other.game_state.castling_rights[color].queenside) {
            return false;
        }
    }

    return true;
}

void Board::show() const {
    for (int i = 0; i < 64; i++) {
        const std::optional<Piece>& piece = this->squares[i].piece;
        if (piece) {
            std::cout << " " << piece->get_char_representation();
        } else {
            std::cout << " _";
        }

        if ((i + 1) % 8 == 0) {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void Board::switch_player_to_move() {
    if (this->game_state.player_to_move == WHITE) {
        this->game_state.player_to_move = BLACK;
    } else {
        this->game_state.player_to_move = WHITE;
    }
}

int Board::get_king_square(Color color) const {
    for (Piece piece : pieces[color]) {
        if (piece.piece_type == KING) {
            return piece.pos;
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}

bool Board::is_insufficient_material() const {
    for (const std::vector<Piece>& pieces : pieces) {
        for (Piece piece : pieces) {
            if (piece.piece_type == PAWN || piece.piece_type == ROOK || piece.piece_type == QUEEN) {
                return false;
            }
        }
    }
    return true;
}

// this will only check if the same position repeats twice
// because otherwise it will do one repetition in a lot of positions,
// before it realizes that it has to do something else
bool Board::is_threefold_repetition() const {
    Board old_board = *this;
    const int history_size = history.size();
    for (int _ = 0; _ < history_size - 1; _++) {

        // if there has been a capture or a pawn move
        // it's impossible to reach the same position again 
        if (old_board.game_state.halfmove_clock == 0) {
            return false;
        }

        old_board.undo();
        if (*this == old_board) {
            return true;
        }         
    }
    return false;
}

bool Board::is_draw_by_fifty_move_rule() const {
    return game_state.halfmove_clock > 100;
}

bool Board::is_lone_king(Color color) const {
    if (pieces[color].size() == 1) {
        return true;
    }
    return false;
}

bool Board::is_endgame() const {
    return game_state.material[WHITE] - KING_VALUE < 1500 && 
           game_state.material[BLACK] - KING_VALUE < 1500;
}
