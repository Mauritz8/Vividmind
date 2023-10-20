#include <cctype>
#include <memory>
#include <stdexcept>
#include <vector>

#include "board.h"
#include "board_utils.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/bishop.h"
#include "pieces/king.h"
#include "pieces/knight.h"
#include "pieces/pawn.h"
#include "pieces/queen.h"
#include "pieces/rook.h"
#include "engine/psqt.h"
#include "square.h"


Piece::Piece(Piece_type piece_type, Color color, int x, int y) {
    this->piece_type = piece_type;
    this->color = color;
    this->x = x;
    this->y = y;
}

bool Piece::operator==(Piece piece) const {
    return piece.x == x && piece.y == y && piece.color == color;
}

std::vector<Move> Piece::get_psuedo_legal_moves(Board& board) const {
    const Square& start = board.get_square(x, y);
    switch (piece_type) {
        case KING: {
            return get_king_psuedo_legal_moves(start, board);
        }
        case QUEEN: {
            return get_queen_psuedo_legal_moves(start, board);
        }
        case ROOK: {
            return get_rook_psuedo_legal_moves(start, board);
        }
        case BISHOP: {
            return get_bishop_psuedo_legal_moves(start, board);
        }
        case KNIGHT: {
            return get_knight_psuedo_legal_moves(start, board);
        }
        case PAWN: {
            return get_pawn_psuedo_legal_moves(*this, board);
        }
    }    
    return {};
}

std::vector<Move> Piece::get_threatened_moves(Board& board) {
    const Square& start = board.get_square(x, y);
    if (piece_type == PAWN) {
        return get_pawn_captures(*this, board);
    }
    if (piece_type == KING) {
        return get_king_threatened_moves(start, board);
    }
    return this->get_psuedo_legal_moves(board);
}

char Piece::get_char_representation() const {
    switch (piece_type) {
        case KING: return 'K';
        case QUEEN: return 'Q';
        case ROOK: return 'R';
        case BISHOP: return 'B';
        case KNIGHT: return 'N';
        case PAWN: return 'p';
    }
}

int Piece::get_value() const {
    switch (piece_type) {
        case KING: return 200;
        case QUEEN: return 9;
        case ROOK: return 5;
        case BISHOP: return 3;
        case KNIGHT: return 3;
        case PAWN: return 1;
    }
}

std::array<std::array<int, 8>, 8> Piece::get_psqt() const {
    switch (piece_type) {
        case KING: return KING_PSQT;
        case QUEEN: return QUEEN_PSQT;
        case ROOK: return ROOK_PSQT;
        case BISHOP: return BISHOP_PSQT;
        case KNIGHT: return KNIGHT_PSQT;
        case PAWN: return PAWN_PSQT;
    }
}

std::vector<Move> get_psuedo_legal_moves_direction(const Square& start, int x_direction, int y_direction, Board& board) {
    std::vector<Move> moves;

    int x = start.x + x_direction;
    int y = start.y + y_direction;
    while (!is_outside_board(x, y)) {
        const Square& end = board.get_square(x, y);
        if (end.piece) {
            if (end.piece->color != start.piece->color) {
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
    throw std::invalid_argument("invalid piece type");
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
