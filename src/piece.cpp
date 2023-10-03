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

bool Piece::is_legal_move(const Move& psuedo_legal, const Board& board, const std::vector<Move>& move_history) const {
    const bool end_square_has_same_color_piece = 
        psuedo_legal.get_end_square().get_piece() &&
        psuedo_legal.get_end_square().get_piece()->get_color() == this->get_color();

    Move psuedo_legal_copy = psuedo_legal;
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;

    psuedo_legal_copy.make_appropriate(board_copy, move_history_copy);
    if (psuedo_legal_copy.get_end_square().is_outside_board() || end_square_has_same_color_piece || is_check(board_copy)) {
        return false;
    }
    return true;
}

std::vector<Move> Piece::get_legal_moves(const Board& board, const std::vector<Move>& move_history) const {
    std::vector<Move> psuedo_legal_moves =  this->get_psuedo_legal_moves(board, move_history);
    for (int i = 0; i < psuedo_legal_moves.size(); i++) {
        Move& move = psuedo_legal_moves.at(i);
        if (!this->is_legal_move(move, board, move_history)) {
            psuedo_legal_moves.erase(psuedo_legal_moves.begin() + i);
        }
    }
    return psuedo_legal_moves;
}

bool Piece::is_psuedo_legal_move(const Move& move, const Board& board, const std::vector<Move>& move_history) const {
    for (const Move& psuedo_legal : this->get_psuedo_legal_moves(board, move_history)) {
        if (move == psuedo_legal) {
            return true;
        }  
    }
    return false;
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
