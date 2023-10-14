#include <cctype>
#include <vector>

#include "board.h"
#include "board_utils.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include "pieces/pawn.h"
#include "square.h"


Piece::Piece(Color color, int x, int y) {
    set_color(color);
    set_x(x);
    set_y(y);
}

Color Piece::get_color() const {
    return color;
}

void Piece::set_color(Color color) {
    this->color = color;
}

int Piece::get_x() const {
    return x;
}
void Piece::set_x(int x) {
    this->x = x;
}
int Piece::get_y() const {
    return y;
}
void Piece::set_y(int y) {
    this->y = y;
}

std::vector<Move> Piece::get_threatened_moves(const Board& board, const std::vector<Move>& move_history) {
    const Pawn* pawn = dynamic_cast<Pawn*>(this);
    if (pawn != nullptr) {
        return pawn->get_threatened_moves(board);
    }
    const King* king = dynamic_cast<King*>(this);
    if (king != nullptr) {
        return king->get_threatened_moves(board);
    }
    return this->get_psuedo_legal_moves(board, move_history);
}

std::vector<Move> Piece::get_psuedo_legal_moves_direction(const Square& start, int x_direction, int y_direction, const Board& board) const {
    std::vector<Move> moves;

    int x = start.get_x() + x_direction;
    int y = start.get_y() + y_direction;
    while (!is_outside_board(x, y)) {
        const Square& end = board.get_square(x, y);
        if (end.get_piece()) {
            if (end.get_piece()->get_color() != this->get_color()) {
                moves.push_back(Move(start, end));
            }
            break;
        }

        moves.push_back(Move(start, end));
        x+= x_direction;
        y+= y_direction;
    }

    return moves;
}

char get_char_representation(Piece_type piece_type) {
    switch (piece_type) {
        case PAWN: return 'p';
        case KNIGHT: return 'N';
        case BISHOP: return 'B';
        case ROOK: return 'R';
        case QUEEN: return 'Q';
        case KING: return 'K';
    }
}

std::optional<Piece_type> get_piece_type(char char_representation) {
    switch (tolower(char_representation)) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
        default: return {};
    }
}

std::optional<Piece_type> get_promotion_piece_type(char char_representation_lowercase) {
    if (char_representation_lowercase == 'n') return KNIGHT;
    if (char_representation_lowercase == 'b') return BISHOP;
    if (char_representation_lowercase == 'r') return ROOK;
    if (char_representation_lowercase == 'q') return QUEEN;
    return {};
}
