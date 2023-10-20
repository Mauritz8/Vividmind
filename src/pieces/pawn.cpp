#include "board.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/pawn.h"
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>


std::unique_ptr<Piece> Pawn::clone() const {
    return std::make_unique<Pawn>(*this);
}

char Pawn::get_char_representation() const {
    return 'p';
}

std::vector<std::vector<int>> Pawn::get_piece_square_table() const {
    return {
        {  0,  0,  0,  0,  0,  0,  0,  0},
        { 50, 50, 50, 50, 50, 50, 50, 50},
        { 10, 10, 20, 30, 30, 20, 10, 10},
        {  5,  5, 10, 25, 25, 10,  5,  5},
        {  0,  0,  0, 20, 20,  0,  0,  0},
        {  5, -5,-10,  0,  0,-10, -5,  5},
        {  5, 10, 10,-20,-20, 10, 10,  5},
        {  0,  0,  0,  0,  0,  0,  0,  0},
    };
}

int Pawn::get_value() const {
    return 1;
}

std::vector<Move> Pawn::get_psuedo_legal_moves(Board& board) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());
    const int direction = this->get_color() == BLACK ? 1 : -1;

    try {
        const Square& end1 = board.get_square(
                start.get_x(),
                start.get_y() + direction);
        if (!end1.get_piece()) {
            moves.push_back(Move(start, end1));

            const Square& end2 = board.get_square(
                    start.get_x(),
                    start.get_y() + 2 * direction);
            const int starting_row = this->get_color() == BLACK ? 1 : 6;
            if (start.get_y() == starting_row && !end2.get_piece()) {
                Move move = Move(start, end2);
                move.is_pawn_two_squares_forward = true;
                moves.push_back(move);
            }
        }
    } catch (const std::invalid_argument& e) {}


    std::vector<Move> captures = this->get_captures(board);
    for (Move& capture : captures) {
        const Square& end = board.get_square(capture.end.x, capture.end.y);
        if (end.get_piece()) {
            moves.push_back(capture);
        } else if (is_valid_en_passant(capture, board)) {
            capture.is_en_passant = true;
            moves.push_back(capture);
        }
    }

    std::vector<Move> other_promotion_moves;
    const std::array<Piece_type, 3> other_promotion_piece_types = {
        ROOK,
        BISHOP,
        KNIGHT
    };
    for (Move& move : moves) {
        if (is_promotion_move(move)) {
            move.is_promotion = true;
            move.promotion_piece = QUEEN;
            for (Piece_type piece_type : other_promotion_piece_types) {
                Move promotion_move = move;
                promotion_move.promotion_piece = piece_type;
                other_promotion_moves.push_back(promotion_move);
            }
        }
    }
    moves.insert(moves.end(), other_promotion_moves.begin(), other_promotion_moves.end());

    return moves;
}

std::vector<Move> Pawn::get_captures(const Board& board) const {
    const int direction = this->get_color() == BLACK ? 1 : -1;

    const Pos start = Pos{this->get_x(), this->get_y()};
    const Pos end1 = Pos{this->get_x() + 1, this->get_y() + direction};
    const Pos end2 = Pos{this->get_x() - 1, this->get_y() + direction};

    std::vector<Move> moves;
    if (!is_outside_board(end1)) {
        moves.push_back(Move(start, end1));
    }
    if (!is_outside_board(end2)) {
        moves.push_back(Move(start, end2));
    }
    return moves;
}

bool Pawn::is_valid_en_passant(const Move& pawn_capture, const Board& board) const {
    if (!board.game_state.en_passant_square.has_value()) {
        return false;
    }
    return pawn_capture.end == board.game_state.en_passant_square.value();
}

bool Pawn::is_promotion_move(const Move& move) const {
    const int promotion_row = this->get_color() == WHITE ? 0 : 7;
    if (move.end.y == promotion_row) {
        return true;
    }
    return false;
}
