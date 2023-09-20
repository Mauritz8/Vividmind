#include <iostream>
#include <optional>
#include <cctype>
#include <string>
#include <vector>

#include "board.h"
#include "move.h"
#include "piece.h"
#include "game_state.h"


Move::Move(const Square& start_square, const Square& end_square) {
    this->set_start_square(start_square);
    this->set_end_square(end_square);
}

Move::Move(const std::string& uci_notation, Board& board) {
    const int start_x = uci_notation[0] - 'a';
    const int start_y = 8 - (uci_notation[1] - '0');
    const int end_x = uci_notation[2] - 'a';
    const int end_y = 8 - (uci_notation[3] - '0');
    const Square& start = board.get_square(start_x, start_y);
    const Square& end = board.get_square(end_x, end_y);
    this->set_start_square(start);
    this->set_end_square(end);

    if (uci_notation.length() == 5) {
        const char promotion_piece = uci_notation[4];
        this->set_promotion_piece(get_promotion_piece_type(promotion_piece));
    }
}

Square Move::get_start_square() const {
    return start_square;
}

void Move::set_start_square(const Square& start_square) {
    this->start_square = start_square;
}

Square Move::get_end_square() const {
    return end_square;
}

void Move::set_end_square(const Square& end_square) {
    this->end_square = end_square;
}

std::optional<Piece> Move::get_captured_piece() const {
    return captured_piece;
}

void Move::set_captured_piece(const std::optional<Piece>& captured_piece) {
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

bool Move::is_threatened_move(const Board& board) const {
    const Square& start_square = this->get_start_square();
    const Square& end_square = this->get_end_square();

    if (start_square.is_outside_board() || end_square.is_outside_board()) {
        return false;
    }

    if (start_square.get_piece() == std::nullopt) {
        return false;
    }

    switch (this->get_start_square().get_piece().value().get_piece_type()) {
        case PAWN:
            return this->is_valid_pawn_move_threat(board);
        case KNIGHT:
            return this->is_valid_knight_move();
        case BISHOP:
            return this->is_valid_bishop_move(board);
        case ROOK:
            return this->is_valid_rook_move(board);
        case QUEEN:
            return this->is_valid_queen_move(board);
        case KING:
            return this->is_valid_king_move();
    }
}

bool Move::leaves_king_in_check(const Board& board, const std::vector<Move>& move_history) const {
    Move move_copy = *this;
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;
    move_copy.make_appropriate(board_copy, move_history_copy);
    if (is_check(board_copy)) {
        return true;
    }
    return false;
}

bool Move::is_promotion_move(const Board& board) const {
    const Piece piece = this->get_start_square().get_piece().value();
    const Piece_type piece_type = piece.get_piece_type();
    const int y_end = this->get_end_square().get_y();
    const int promotion_row = piece.get_color() == WHITE ? 0 : 7;

    if (piece_type == PAWN && y_end == promotion_row) {
        return true;
    }
    return false;
}

bool Move::is_valid_promotion(const Board& board) const {
    const bool is_correct_promotion_piece = 
        this->get_promotion_piece() == KNIGHT ||
        this->get_promotion_piece() == BISHOP ||
        this->get_promotion_piece() == ROOK   ||
        this->get_promotion_piece() == QUEEN;


    if (this->is_promotion_move(board) && this->is_valid_pawn_move(board) && is_correct_promotion_piece) {
        return true;
    }
    return false;
}

bool Move::is_legal(const Board& board, const std::vector<Move>& move_history) const {
    if (!this->validate_basic(board)) {
        return false;
    }
    if (this->leaves_king_in_check(board, move_history)) {
        return false;
    }

    if (this->is_valid_promotion(board)) {
        return true;
    }
    if (this->is_valid_castling(board, move_history)) {
        return true;
    }
    if (this->is_valid_en_passant(board, move_history)) {
        return true;
    }

    if (!this->is_valid_piece_movement(board)) {
        return false;
    }

    return true;
}

void Move::make_appropriate(Board& board, std::vector<Move>& move_history) {
    if (this->is_valid_castling(board, move_history)) {
        this->set_castling_move(true);
        this->set_promotion(false);
        this->set_en_passant(false);
        this->make_castling(board);
    } else if (this->is_valid_en_passant(board, move_history)) {
        this->set_en_passant(true);
        this->set_castling_move(false);
        this->set_promotion(false);
        this->make_en_passant(board);
    } else if (this->is_valid_promotion(board)) {
        this->set_promotion(true);
        this->set_castling_move(false);
        this->set_en_passant(false);
        this->make_promotion(board);
    } else {
        this->set_castling_move(false);
        this->set_promotion(false);
        this->set_en_passant(false);
        this->make(board);
    }

    move_history.push_back(*this);
    board.switch_player_to_move();
}

void Move::undo_appropriate(Board& board, std::vector<Move>& move_history) {
    const Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    if (!end_square.get_piece().has_value()) {
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
    if (end_square.get_piece().has_value()) {
        this->captured_piece = end_square.get_piece();
    } else {
        this->captured_piece = {};
    }
    end_square.set_piece(start_square.get_piece());
    start_square.set_piece({});
}

void Move::undo(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y()); 
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y()); 
    start_square.set_piece(end_square.get_piece());
    if (this->get_captured_piece().has_value()) {
        end_square.set_piece(this->get_captured_piece());
    } else {
        end_square.set_piece({});
    }
}

static bool is_same_line(const Square& square1, const Square& square2) {
    return square1.get_x() == square2.get_x() || square1.get_y() == square2.get_y();
}

static bool is_same_diagonal(const Square& square1, const Square& square2) {
    return abs(square1.get_x() - square2.get_x()) == abs(square1.get_y() - square2.get_y());
}

static bool is_clear_line(const Square& square1, const Square& square2, const Board& board) {
    int x_direction = 0;
    int y_direction = 0;

    if (square1.get_x() != square2.get_x()) {
        x_direction = (square2.get_x() - square1.get_x()) > 0 ? 1 : -1;
    } else if (square1.get_y() != square2.get_y()) {
        y_direction = (square2.get_y() - square1.get_y()) > 0 ? 1 : -1;
    }

    int x = square1.get_x() + x_direction;
    int y = square1.get_y() + y_direction;
    while (x != square2.get_x() || y != square2.get_y()) {
        if (board.get_square(x, y).get_piece().has_value()) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

static bool is_clear_diagonal(const Square& square1, const Square& square2, const Board& board) {
    if (square1 == square2) {
        return true;
    }

    const int x_direction = (square2.get_x() - square1.get_x()) > 0 ? 1 : -1;
    const int y_direction = (square2.get_y() - square1.get_y()) > 0 ? 1 : -1;

    int x = square1.get_x() + x_direction;
    int y = square1.get_y() + y_direction;
    while (x != square2.get_x() && y != square2.get_y()) {
        if (board.get_square(x, y).get_piece().has_value()) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
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
    const int direction = this->get_start_square().get_piece().value().get_color() == BLACK ? 1 : -1;

    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    const int y_diff = this->get_end_square().get_y() - this->get_start_square().get_y();

    const bool end_square_is_empty = this->get_end_square().get_piece() == std::nullopt;
    const bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && end_square_is_empty;

    const int starting_row = this->get_start_square().get_piece().value().get_color() == BLACK ? 1 : 6;
    const bool is_on_starting_row = this->get_start_square().get_y() == starting_row;
    const bool one_square_forward_is_empty = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y() + direction).get_piece() == std::nullopt;
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

bool Move::validate_basic(const Board& board) const {
    const Square& start_square = this->get_start_square();
    const Square& end_square = this->get_end_square();

    if (!start_square.get_piece().has_value()) {
        return false;
    }

    if (start_square.get_piece().value().get_color() != board.get_player_to_move()) {
        return false;
    }

    if (start_square.is_outside_board() || end_square.is_outside_board()) {
        return false;
    }

    if (this->get_start_square().get_piece() == std::nullopt) {
        return false;
    }

    if (this->get_end_square().get_piece() && this->get_start_square().get_piece().value().get_color() == this->get_end_square().get_piece().value().get_color()) {
        return false;
    }
    return true;
}

bool Move::is_valid_piece_movement(const Board& board) const {
    switch (this->get_start_square().get_piece().value().get_piece_type()) {
        case PAWN:
            return is_valid_pawn_move(board);
        case KNIGHT:
            return is_valid_knight_move();
        case BISHOP:
            return is_valid_bishop_move(board);
        case ROOK:
            return is_valid_rook_move(board);
        case QUEEN:
            return is_valid_queen_move(board);
        case KING:
            return is_valid_king_move();
    }
}

static bool has_castling_pieces_moved(const std::vector<Move>& move_history, int starting_row, int king_x, int rook_x) {
    for (int i = 0; i < move_history.size(); i++) {
        const Move& played_move = move_history.at(i);
        if (played_move.get_start_square().get_y() == starting_row) {
            if (played_move.get_start_square().get_x() == king_x || played_move.get_start_square().get_x() == rook_x) {
                return true;
            }
        }
    }
    return false;
}

bool Move::passes_through_check_when_castling(const Board& board, const std::vector<Move>& move_history) const {
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;
    const int row = this->get_start_square().get_y();
    const int start_x = this->get_start_square().get_x();
    const int end_x = this->get_end_square().get_x();
    const int direction = end_x - start_x > 0 ? 1 : -1;

    int x = start_x;
    Move submove;
    while (x != end_x) {
        submove.start_square = board_copy.get_square(x, row);
        submove.end_square = board_copy.get_square(x + direction, row);
        submove.make_appropriate(board_copy, move_history_copy);
        if (is_check(board_copy)) {
            return true;
        }
        x += direction;
    }
    return false;
}

bool Move::is_valid_castling(const Board& board, const std::vector<Move>& move_history) const {
    const Color color = this->get_start_square().get_piece().value().get_color();
    const int starting_row = color == WHITE ? 7 : 0;

    if (this->end_square.get_y() != starting_row) {
        return false;
    }
    const int king_x = 4;
    if (this->get_start_square().get_x() != king_x || this->get_start_square().get_y() != starting_row) {
        return false;
    }
    int rook_x;
    if (this->end_square.get_x() == 6) {
        rook_x = 7;
    } else if (this->end_square.get_x() == 2) {
        rook_x = 0;
    } else {
        return false;
    }

    if (has_castling_pieces_moved(move_history, starting_row, king_x, rook_x)) {
        return false;
    }
    
    const Square& king_square = board.get_square(king_x, starting_row);
    const Square& rook_square = board.get_square(rook_x, starting_row);
    if (!is_clear_line(king_square, rook_square, board)) {
        return false;
    }
    if (is_check(board)) {
        return false;
    }
    if (this->passes_through_check_when_castling(board, move_history)) {
        return false;
    }

    return true;
}

Move Move::get_castling_rook_move(const Board& board) const {
    Move rook_move;
    const int row = this->get_start_square().get_y();
    const int move_direction = this->get_end_square().get_x() - this->get_start_square().get_x(); 
    if (move_direction > 0) {
        // kingside castling
        rook_move.set_start_square(board.get_square(7, row));
        rook_move.set_end_square(board.get_square(5, row));
    } else {
        // queenside castling
        rook_move.set_start_square(board.get_square(0, row));
        rook_move.set_end_square(board.get_square(3, row));
    }
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

bool Move::is_valid_en_passant(const Board& board, const std::vector<Move>& move_history) const {
    if (this->get_start_square().get_piece().value().get_piece_type() != PAWN) {
        return false;
    }
    const int direction = this->get_start_square().get_piece().value().get_color() == BLACK ? 1 : -1;
    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    const int y_diff = this->get_end_square().get_y() - this->get_start_square().get_y();

    const bool is_diagonal_pawn_move = abs(x_diff) == 1 && y_diff == direction;
    const std::optional<Piece>& adjacent_piece = board.get_square(this->get_start_square().get_x() + x_diff, this->get_start_square().get_y()).get_piece();
    const bool has_pawn_adjacent = adjacent_piece.has_value() && adjacent_piece.value().get_piece_type() == PAWN;
    const bool is_adjacent_pawn_opponents_piece = adjacent_piece.has_value() && adjacent_piece.value().get_color() != this->get_start_square().get_piece().value().get_color();

    if (move_history.size() == 0) {
        return false;
    }
    const Move& previous_move = move_history.back();
    const int y_diff_previous_move = previous_move.get_end_square().get_y() - previous_move.get_start_square().get_y();

    return is_diagonal_pawn_move &&
           has_pawn_adjacent &&
           is_adjacent_pawn_opponents_piece &&
           abs(y_diff_previous_move) == 2;
}

void Move::make_en_passant(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y());
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    end_square.set_piece(start_square.get_piece());
    start_square.set_piece({});

    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    Square& captured_square = board.get_square(this->get_start_square().get_x() + x_diff, this->get_start_square().get_y());
    this->set_captured_piece(captured_square.get_piece());
    captured_square.set_piece({});
}

void Move::undo_en_passant(Board& board) {
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y());
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    start_square.set_piece(end_square.get_piece());
    end_square.set_piece({});

    const int x_diff = this->get_end_square().get_x() - this->get_start_square().get_x();
    Square& captured_square = board.get_square(this->get_start_square().get_x() + x_diff, this->get_start_square().get_y());
    captured_square.set_piece(this->captured_piece);
}

void Move::make_promotion(Board& board) {
    this->make(board);
    Square& end_square = board.get_square(this->get_end_square().get_x(), this->get_end_square().get_y());
    end_square.get_piece().value().set_piece_type(this->get_promotion_piece().value());
}

void Move::undo_promotion(Board& board) {
    this->undo(board);
    Square& start_square = board.get_square(this->get_start_square().get_x(), this->get_start_square().get_y());
    start_square.get_piece().value().set_piece_type(PAWN);
}
