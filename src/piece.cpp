#include <cctype>
#include <memory>
#include <stdexcept>
#include <vector>

#include "board.h"
#include "board_utils.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include "pieces/pawn.h"
#include "engine/psqt.h"
#include "square.h"


Piece::Piece(Piece_type piece_type, Color color, Pos pos) {
    this->piece_type = piece_type;
    this->color = color;
    this->pos.x = pos.x;
    this->pos.y = pos.y;
}

bool Piece::operator==(Piece piece) const {
    return piece.pos.x == pos.x && piece.pos.y == pos.y && piece.color == color;
}

std::vector<Move> Piece::get_psuedo_legal_moves(Board& board) const {
    const Square& start = board.get_square(pos.x, pos.y);
    switch (piece_type) {
        case KING: {
            return get_king_psuedo_legal_moves(board);
        }
        case QUEEN: {
            return get_queen_psuedo_legal_moves(board);
        }
        case ROOK: {
            return get_rook_psuedo_legal_moves(board);
        }
        case BISHOP: {
            return get_bishop_psuedo_legal_moves(board);
        }
        case KNIGHT: {
            return get_knight_psuedo_legal_moves(board);
        }
        case PAWN: {
            return get_pawn_psuedo_legal_moves(*this, board);
        }
    }    
    return {};
}

std::vector<Move> Piece::get_threatened_moves(Board& board) {
    const Square& start = board.get_square(pos.x, pos.y);
    if (piece_type == PAWN) {
        return get_pawn_captures(*this, board);
    }
    if (piece_type == KING) {
        return get_king_threatened_moves(*this, board);
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

int Piece::get_psqt_score() const {
    int y = color == WHITE ? pos.y : 7 - pos.y;
    switch (piece_type) {
        case KING: return KING_PSQT[y][pos.x];
        case QUEEN: return QUEEN_PSQT[y][pos.x];
        case ROOK: return ROOK_PSQT[y][pos.x];
        case BISHOP: return BISHOP_PSQT[y][pos.x];
        case KNIGHT: return KNIGHT_PSQT[y][pos.x];
        case PAWN: return PAWN_PSQT[y][pos.x];
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


std::vector<Move> Piece::get_bishop_psuedo_legal_moves(Board& board) const {
    std::vector<Move> moves;

    std::vector<Move> up_right = get_psuedo_legal_moves_direction(1, 1, board);
    std::vector<Move> down_right = get_psuedo_legal_moves_direction(1, -1, board);
    std::vector<Move> down_left = get_psuedo_legal_moves_direction(-1, -1, board);
    std::vector<Move> up_left = get_psuedo_legal_moves_direction(-1, 1, board);

    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));

    return moves;
}

std::vector<Move> Piece::get_rook_psuedo_legal_moves(Board& board) const {
    std::vector<Move> moves;

    std::vector<Move> up = get_psuedo_legal_moves_direction(0, 1, board);
    std::vector<Move> right = get_psuedo_legal_moves_direction(1, 0, board);
    std::vector<Move> down = get_psuedo_legal_moves_direction(0, -1, board);
    std::vector<Move> left = get_psuedo_legal_moves_direction(-1, 0, board);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(left), std::end(left));

    return moves;
}

std::vector<Move> Piece::get_queen_psuedo_legal_moves(Board& board) const {
    std::vector<Move> moves;

    std::vector<Move> up = get_psuedo_legal_moves_direction(0, 1, board);
    std::vector<Move> up_right = get_psuedo_legal_moves_direction(1, 1, board);
    std::vector<Move> right = get_psuedo_legal_moves_direction(1, 0, board);
    std::vector<Move> down_right = get_psuedo_legal_moves_direction(1, -1, board);
    std::vector<Move> down = get_psuedo_legal_moves_direction(0, -1, board);
    std::vector<Move> down_left = get_psuedo_legal_moves_direction(-1, -1, board);
    std::vector<Move> left = get_psuedo_legal_moves_direction(-1, 0, board);
    std::vector<Move> up_left = get_psuedo_legal_moves_direction(-1, 1, board);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(left), std::end(left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));


    return moves;
}

std::vector<Move> Piece::get_knight_psuedo_legal_moves(Board& board) const {
    const std::array<Pos, 8> end_squares = {
        Pos{pos.x + 1, pos.y + 2},
        Pos{pos.x + 1, pos.y - 2},
        Pos{pos.x - 1, pos.y + 2},
        Pos{pos.x - 1, pos.y - 2},
        Pos{pos.x + 2, pos.y + 1},
        Pos{pos.x + 2, pos.y - 1},
        Pos{pos.x - 2, pos.y + 1},
        Pos{pos.x - 2, pos.y - 1}
    };

    std::vector<Move> moves;
    for (Pos end : end_squares) {
        if (!is_outside_board(end) && !is_occupied_by_color(end, color, board)) {
            moves.push_back(Move(pos, end));
        }
    }
    return moves;
}

std::vector<Move> Piece::get_king_psuedo_legal_moves(Board& board) const {
    std::vector<Move> moves = get_king_threatened_moves(*this, board);
    std::vector<Move> castling_moves = get_castling_moves(board);
    moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
    return moves;
}

std::vector<Move> Piece::get_psuedo_legal_moves_direction(int x_direction, int y_direction, Board& board) const {
    std::vector<Move> moves;

    int x = this->pos.x + x_direction;
    int y = this->pos.y + y_direction;
    while (!is_outside_board(x, y)) {
        const Square& end = board.get_square(x, y);
        if (end.piece) {
            if (end.piece->color != this->color) {
                moves.push_back(Move(this->pos, Pos{x, y}));
            }
            break;
        }

        moves.push_back(Move(this->pos, Pos{x, y}));
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
