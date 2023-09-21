#include <cctype>
#include <vector>

#include "board.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
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

std::vector<Move> Piece::get_legal_moves(const Board& board, const std::vector<Move>& move_history) const {
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;

    std::vector<Move> legal_moves =  this->get_psuedo_legal_moves(board);
    for (int i = 0; i < legal_moves.size(); i++) {
        Move& move = legal_moves.at(i);
        const bool end_square_has_same_color_piece = 
            move.get_end_square().get_piece() &&
            move.get_end_square().get_piece()->get_color() == this->get_color();

        move.make_appropriate(board_copy, move_history_copy);
        if (move.get_end_square().is_outside_board() || end_square_has_same_color_piece || is_check(board_copy)) {
            legal_moves.erase(legal_moves.begin() + i);
        }
        move.undo_appropriate(board_copy, move_history_copy);
    }
    return legal_moves;
}

std::vector<Move> Piece::get_psuedo_legal_moves_direction(const Square& start, int x_direction, int y_direction, const Board& board) const {
    std::vector<Move> moves;

    int x = start.get_x() + x_direction;
    int y = start.get_y() + y_direction;
    int x_final = x_direction == 1 ? 7 : 0;
    int y_final = y_direction == 1 ? 7 : 0;
    while (x != x_final && y != y_final) {
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
