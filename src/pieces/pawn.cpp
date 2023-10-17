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

std::vector<Move> Pawn::get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const {
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
                moves.push_back(Move(start, end2));
            }
        }
    } catch (const std::invalid_argument& e) {}


    std::vector<Move> captures;
    try {
        const Square& end3 = board.get_square(start.get_x() + 1, start.get_y() + direction);
        captures.push_back(Move(start, end3));
    } catch (const std::invalid_argument& e) {}
    try {
        const Square& end4 = board.get_square(start.get_x() - 1, start.get_y() + direction);
        captures.push_back(Move(start, end4));
    } catch (const std::invalid_argument& e) {}

    for (Move& capture : captures) {
        const bool is_valid_capture = 
            capture.get_end_square().get_piece() &&
            capture.get_end_square().get_piece()->get_color() != this->get_color();
        if (is_valid_capture) {
            moves.push_back(capture);
        } else if (is_valid_en_passant(capture, board, move_history)) {
            capture.set_en_passant(true);
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
            move.set_promotion(true);
            move.set_promotion_piece(QUEEN);
            for (Piece_type piece_type : other_promotion_piece_types) {
                Move promotion_move = move;
                promotion_move.set_promotion_piece(piece_type);
                other_promotion_moves.push_back(promotion_move);
            }
        }
    }
    moves.insert(moves.end(), other_promotion_moves.begin(), other_promotion_moves.end());

    return moves;
}

std::vector<Move> Pawn::get_threatened_moves(const Board& board) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());
    const int direction = this->get_color() == BLACK ? 1 : -1;

    try {
        const Square& end1 = board.get_square(this->get_x() + 1, this->get_y() + direction);
        moves.push_back(Move(start, end1));
    } catch (const std::invalid_argument& e) {}
    try {
        const Square& end2 = board.get_square(this->get_x() - 1, this->get_y() + direction);
        moves.push_back(Move(start, end2));
    } catch (const std::invalid_argument& e) {}

    return moves;
}

bool Pawn::is_valid_en_passant(const Move& pawn_capture, const Board& board, const std::vector<Move>& move_history) const {
    const int direction = this->get_color() == BLACK ? 1 : -1;

    const std::shared_ptr<Piece>& adjacent_piece = board.get_square(pawn_capture.get_end_square().get_x(), pawn_capture.get_start_square().get_y()).get_piece();
    const bool has_pawn_adjacent = adjacent_piece && dynamic_cast<Pawn*>(adjacent_piece.get()) != nullptr;
    const bool is_adjacent_pawn_opponents_piece = adjacent_piece && adjacent_piece->get_color() != pawn_capture.get_start_square().get_piece()->get_color();

    if (move_history.size() == 0) {
        return false;
    }
    const Move& previous_move = move_history.back();
    const int y_diff_previous_move = previous_move.get_end_square().get_y() - previous_move.get_start_square().get_y();

    const std::shared_ptr<Piece>& last_move_piece = board.get_square(previous_move.get_end_square().get_x(), previous_move.get_end_square().get_y()).get_piece();

    return has_pawn_adjacent &&
           is_adjacent_pawn_opponents_piece &&
           abs(y_diff_previous_move) == 2 &&
           adjacent_piece.get() == last_move_piece.get();
} 

bool Pawn::is_promotion_move(const Move& move) const {
    const int y_end = move.get_end_square().get_y();
    const int promotion_row = this->get_color() == WHITE ? 0 : 7;
    if (y_end == promotion_row) {
        return true;
    }
    return false;
}
