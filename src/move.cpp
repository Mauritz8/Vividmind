#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <cctype>
#include <string>
#include <vector>

#include "board.h"
#include "board_utils.h"
#include "move.h"
#include "piece.h"
#include "game_state.h"
#include "pieces/bishop.h"
#include "pieces/knight.h"
#include "pieces/pawn.h"
#include "pieces/queen.h"
#include "pieces/rook.h"


Move::Move(const Square& start_square, const Square& end_square) 
    : start_square(start_square),
      end_square(end_square) 
{
    this->set_castling_move(false);
    this->set_promotion(false);
    this->set_en_passant(false);
}

Move Move::get_from_uci_notation(const std::string& uci_notation, const Board& board) {
    const int start_x = uci_notation[0] - 'a';
    const int start_y = 8 - (uci_notation[1] - '0');
    const int end_x = uci_notation[2] - 'a';
    const int end_y = 8 - (uci_notation[3] - '0');
    const Square& start = board.get_square(start_x, start_y);
    const Square& end = board.get_square(end_x, end_y);
    Move move = Move(start, end);

    if (uci_notation.length() == 5) {
        const char promotion_piece = uci_notation[4];
        move.set_promotion_piece(get_promotion_piece_type(promotion_piece));
    }
    return move;
}

Move::Move(const Move& move) 
    : start_square(move.start_square),
      end_square(move.end_square)
{
    this->castling_move = move.castling_move;
    this->promotion = move.promotion;
    this->promotion_piece = move.promotion_piece;
    this->en_passant = move.en_passant;
    if (move.get_captured_piece_ref()) {
        this->captured_piece = move.captured_piece;
    } else {
        this->captured_piece = nullptr;
    }
}

const Square& Move::get_start_square() const {
    return start_square;
}

void Move::set_start_square(const Square& start_square) {
    this->start_square = start_square;
}

const Square& Move::get_end_square() const {
    return end_square;
}

void Move::set_end_square(const Square& end_square) {
    this->end_square = end_square;
}

const std::shared_ptr<Piece>& Move::get_captured_piece_ref() const {
    return captured_piece;
}

std::shared_ptr<Piece> Move::get_captured_piece() {
    return captured_piece;
}

void Move::set_captured_piece(std::shared_ptr<Piece> captured_piece) {
    this->captured_piece = captured_piece; 
}

bool Move::is_castling_move() const {
    return castling_move;
}

void Move::set_castling_move(bool castling_move) {
    this->castling_move = castling_move;
}

bool Move::is_promotion() const {
    return promotion;
}

void Move::set_promotion(bool promotion) {
    this->promotion = promotion;
}

std::optional<Piece_type> Move::get_promotion_piece() const {
    return promotion_piece;
}

void Move::set_promotion_piece(const std::optional<Piece_type> promotion_piece) {
    this->promotion_piece = promotion_piece;
}

bool Move::is_en_passant() const {
    return en_passant;
}

void Move::set_en_passant(bool en_passant) {
    this->en_passant = en_passant;
}

Move Move::operator=(const Move& move) {
    this->set_start_square(move.get_start_square());
    this->set_end_square(move.get_end_square());
    this->set_castling_move(move.is_castling_move());
    if (move.get_captured_piece_ref()) {
        this->captured_piece = move.captured_piece;
    } else {
        this->captured_piece = nullptr;
    }
    this->set_promotion(move.is_promotion());
    this->set_promotion_piece(move.get_promotion_piece());
    this->set_en_passant(move.is_en_passant());
    return *this;
}

bool Move::operator==(const Move& move) const {
    return this->get_start_square() == move.get_start_square()
        && this->get_end_square() == move.get_end_square()
        && this->get_start_square().get_piece() == move.get_start_square().get_piece();
}

bool Move::leaves_king_in_check(const Board& board, const std::vector<Move>& move_history) const {
    Move move_copy = *this;
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;
    move_copy.make_appropriate(board_copy, move_history_copy);
    if (is_check(board_copy, move_history_copy)) {
        return true;
    }
    return false;
}

void Move::make_appropriate(Board& board, std::vector<Move>& move_history) {
    if (this->is_castling_move()) {
        this->make_castling(board);
    } else if (this->is_en_passant()) {
        this->make_en_passant(board);
    } else if (this->is_promotion()) {
        this->make_promotion(board);
    } else {
        this->make(board);
    }

    move_history.push_back(*this);
    board.switch_player_to_move();
}

void Move::undo_appropriate(Board& board, std::vector<Move>& move_history) {
    const Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    if (!end_square.get_piece()) {
        std::cout << "Can't undo move\n"; 
        return;
    }

    if (this->is_castling_move()) {
        this->undo_castling(board);
    } else if (this->is_en_passant()) {
        this->undo_en_passant(board);
    } else if (this->is_promotion()) {
        this->undo_promotion(board);
    } else {
        this->undo(board);
    }

    move_history.pop_back();
    board.switch_player_to_move();
}

std::string Move::to_uci_notation() const {
    const std::string files = "abcdefgh";
    const std::string ranks = "87654321";

    std::string uci_notation;
    uci_notation += files[this->get_start_square().get_x()];
    uci_notation += ranks[this->get_start_square().get_y()];
    uci_notation += files[this->get_end_square().get_x()];
    uci_notation += ranks[this->get_end_square().get_y()];

    if (this->get_promotion_piece().has_value()) {
        uci_notation += tolower(get_char_representation(this->get_promotion_piece().value()));
    }
    return uci_notation;
}

void Move::make(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y()); 
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y()); 
    std::shared_ptr<Piece> piece = std::move(end_square.get_piece());
    if (piece) {
        this->set_captured_piece(std::move(piece));
    }
    start_square.move_piece(end_square);
}

void Move::undo(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y()); 
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y()); 
    end_square.move_piece(start_square);
    std::shared_ptr<Piece> captured_piece = std::move(this->get_captured_piece());
    if (captured_piece) {
        end_square.set_piece(std::move(captured_piece));
    }
}

bool Move::is_valid_rook_move(const Board& board) const {
    const Square& start_square = this->get_start_square();
    const Square& end_square = this->get_end_square();
    if (!is_same_line(start_square, end_square)) {
        return false;
    }
    if (!is_clear_line(start_square, end_square, board)) {
        return false;
    }
    return true;
}

bool Move::is_valid_bishop_move(const Board& board) const {
    const Square& start_square = this->get_start_square();
    const Square& end_square = this->get_end_square();
    if (!is_same_diagonal(start_square, end_square)) {
        return false;
    }
    if (!is_clear_diagonal(start_square, end_square, board)) {
        return false;
    }
    return true;
}

bool Move::is_valid_queen_move(const Board& board) const {
    if (this->is_valid_rook_move(board)) {
        return true;
    }
    if (this->is_valid_bishop_move(board)) {
        return true;
    }
    return false;
}

bool Move::is_valid_knight_move() const {
    const int start_x = this->get_start_square().get_x();
    const int start_y = this->get_start_square().get_y();
    const int end_x = this->get_end_square().get_x();
    const int end_y = this->get_end_square().get_y();

    if (abs(end_x - start_x) == 1 && abs(end_y - start_y) == 2) {
        return true;
    }
    if (abs(end_x - start_x) == 2 && abs(end_y - start_y) == 1) {
        return true;
    }
    return false;
}

bool Move::is_valid_king_move() const {
    const int x_diff = abs(this->get_start_square().get_x() - this->get_end_square().get_x());
    const int y_diff = abs(this->get_start_square().get_y() - this->get_end_square().get_y());
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

bool Move::is_valid_pawn_move(const Board& board) const {
    const int direction = this->get_start_square().get_piece()->get_color() == BLACK ? 1 : -1;

    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    const int y_diff = this->get_end_square().get_y() - this->get_start_square().get_y();

    const bool end_square_is_empty = this->get_end_square().get_piece() == nullptr;
    const bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && end_square_is_empty;

    const int starting_row = this->get_start_square().get_piece()->get_color() == BLACK ? 1 : 6;
    const bool is_on_starting_row = this->get_start_square().get_y() == starting_row;
    const bool one_square_forward_is_empty = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y() + direction).get_piece() == nullptr;
    const bool is_valid_move_two_squares_forward = x_diff == 0 && y_diff == 2 * direction && is_on_starting_row && one_square_forward_is_empty && end_square_is_empty;

    const bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square_is_empty;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

bool Move::is_valid_pawn_move_threat(const Board& board) const {
    const int direction = this->get_start_square().get_piece()->get_color() == BLACK ? 1 : -1;
    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    const int y_diff = this->get_end_square().get_y() - this->get_start_square().get_y();

    const bool is_threatening_diagonally = abs(x_diff) == 1 && y_diff == direction;

    if (is_threatening_diagonally) {
        return true;
    }
    return false;
}

Move Move::get_castling_rook_move(const Board& board) const {
    const int row = this->get_start_square().get_y();
    const int move_direction = this->get_end_square().get_x() - this->get_start_square().get_x(); 

    int start_x;
    int end_x;
    if (move_direction > 0) {
        // kingside castling
        start_x = 7;
        end_x = 5;
    } else {
        // queenside castling
        start_x = 0;
        end_x = 3;
    }
    Move rook_move = Move(board.get_square(7, row), board.get_square(5, row));
    rook_move.set_captured_piece({});
    return rook_move;
}

void Move::make_castling(Board& board) {
    this->make(board);
    Move rook_move = this->get_castling_rook_move(board);
    rook_move.make(board);
}

void Move::undo_castling(Board& board) {
    this->undo(board);
    Move rook_move = this->get_castling_rook_move(board);
    rook_move.undo(board);
}

void Move::make_en_passant(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y());
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    start_square.move_piece(end_square);

    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    Square& captured_square = board.get_square(this->get_start_square().get_x() + x_diff, this->get_start_square().get_y());
    this->set_captured_piece(captured_square.get_piece());
    captured_square.get_piece().reset();
}

void Move::undo_en_passant(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y());
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    end_square.move_piece(start_square);

    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    Square& captured_square = board.get_square(this->get_start_square().get_x() + x_diff, this->get_start_square().get_y());
    captured_square.set_piece(this->get_captured_piece());
}

void Move::make_promotion(Board& board) {
    this->make(board);
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    if (!this->get_promotion_piece()) {
        return;
    }
    switch (this->get_promotion_piece().value()) {
        case QUEEN: {
            std::shared_ptr<Piece> piece = end_square.get_piece();
            piece.reset(dynamic_cast<Queen*>(piece.get()));
            break;
        }
        case ROOK: {
            std::shared_ptr<Piece> piece = end_square.get_piece();
            piece.reset(dynamic_cast<Rook*>(piece.get()));
            break;
        }
        case BISHOP: {
            std::shared_ptr<Piece> piece = end_square.get_piece();
            piece.reset(dynamic_cast<Bishop*>(piece.get()));
            break;
        }
        case KNIGHT: {
            std::shared_ptr<Piece> piece = end_square.get_piece();
            piece.reset(dynamic_cast<Knight*>(piece.get()));
            break;
        }
        default: break;
    }
}

void Move::undo_promotion(Board& board) {
    this->undo(board);
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y());
    std::shared_ptr<Piece> piece = start_square.get_piece();
    *piece = *dynamic_cast<Pawn*>(piece.get());
}
