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
#include "pieces/king.h"
#include "pieces/knight.h"
#include "pieces/pawn.h"
#include "pieces/queen.h"
#include "pieces/rook.h"


Move::Move(int start_x, int start_y, int end_x, int end_y) {
    this->start = {start_x, start_y};
    this->end = {end_x, end_y};
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move::Move(const Square& start, const Square& end) {
    this->start = {start.get_x(), start.get_y()};
    this->end = {end.get_x(), end.get_y()};
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move Move::get_from_uci_notation(const std::string& uci_notation, const Board& board) {
    const int start_x = uci_notation[0] - 'a';
    const int start_y = 8 - (uci_notation[1] - '0');
    const int end_x = uci_notation[2] - 'a';
    const int end_y = 8 - (uci_notation[3] - '0');
    Move move = Move(start_x, start_y, end_x, end_y);

    const Square& start = board.get_square(start_x, start_y);
    const Square& end = board.get_square(end_x, end_y);
    King* king = dynamic_cast<King*>(start.get_piece().get());
    Pawn* pawn = dynamic_cast<Pawn*>(start.get_piece().get());
    if (king && move.is_valid_castling_move()) {
        move.is_castling_move = true;
    } else if (pawn) {
        if (pawn->is_valid_en_passant(move, board)) {
            move.is_en_passant = true;
        } else if (pawn->is_promotion_move(move)) {
            move.is_promotion = true;
            const char promotion_piece = uci_notation[4];
            move.promotion_piece = get_promotion_piece_type(promotion_piece);
        } else if (abs(move.end.y - move.start.y) == 2) {
            move.is_pawn_two_squares_forward = true;
        }
    }
    return move;
}

Move::Move(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->is_castling_move = move.is_castling_move;
    this->is_promotion = move.is_promotion;
    this->promotion_piece = move.promotion_piece;
    this->is_en_passant = move.is_en_passant;
    this->captured_piece = move.captured_piece;
    this->is_pawn_two_squares_forward = move.is_pawn_two_squares_forward;
}


Move Move::operator=(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->captured_piece = move.captured_piece;
    this->is_castling_move = move.is_castling_move;
    this->is_promotion = move.is_promotion;
    this->promotion_piece = move.promotion_piece;
    this->is_en_passant = move.is_en_passant;
    this->is_pawn_two_squares_forward = move.is_pawn_two_squares_forward;
    return *this;
}

bool Move::leaves_king_in_check(Board& board) const {
    Move move_copy = *this;
    const Color player_to_move = board.game_state.player_to_move;
    move_copy.make_appropriate(board);
    if (is_in_check(player_to_move, board)) {
        move_copy.undo_appropriate(board);
        return true;
    }
    move_copy.undo_appropriate(board);
    return false;
}

void Move::make_appropriate(Board& board) {
    board.history.push_back(board.game_state);
    if (this->is_castling_move) {
        this->make_castling(board);
    } else if (this->is_en_passant) {
        this->make_en_passant(board);
    } else if (this->is_promotion) {
        this->make_promotion(board);
    } else if (this->is_pawn_two_squares_forward) {
        this->make_pawn_two_squares_forward(board);
    } else {
        this->make(board);
    }

    board.switch_player_to_move();
}

void Move::undo_appropriate(Board& board) {
    const Square& end_square = board.get_square(this->end.x, this->end.y);
    if (!end_square.get_piece()) {
        std::cout << "Can't undo move\n"; 
        return;
    }

    if (this->is_castling_move) {
        this->undo_castling(board);
    } else if (this->is_en_passant) {
        this->undo_en_passant(board);
    } else if (this->is_promotion) {
        this->undo_promotion(board);
    } else if (this->is_pawn_two_squares_forward) {
        this->undo_pawn_two_squares_forward(board);
    } else {
        this->undo(board);
    }

    board.game_state = board.history.at(board.history.size() - 1);
    board.history.pop_back();
}

std::string Move::to_uci_notation() const {
    const std::string files = "abcdefgh";
    const std::string ranks = "87654321";

    std::string uci_notation;
    uci_notation += files[this->start.x];
    uci_notation += ranks[this->start.y];
    uci_notation += files[this->end.x];
    uci_notation += ranks[this->end.y];

    if (this->promotion_piece.has_value()) {
        uci_notation += tolower(get_char_representation(this->promotion_piece.value()));
    }
    return uci_notation;
}

void Move::make(Board& board) {
    Square& start_square = board.get_square(this->start.x, this->start.y); 
    Square& end_square = board.get_square(this->end.x, this->end.y); 
    std::shared_ptr<Piece> captured_piece = std::move(end_square.get_piece());
    if (captured_piece) {
        board.remove_piece(captured_piece);
        this->captured_piece = std::move(captured_piece);
    }
    start_square.move_piece(end_square);

    board.game_state.en_passant_square = {};
    this->update_castling_rights(board);
}

void Move::undo(Board& board) {
    Square& start_square = board.get_square(this->start.x, this->start.y); 
    Square& end_square = board.get_square(this->end.x, this->end.y); 
    end_square.move_piece(start_square);
    std::shared_ptr<Piece> captured_piece = std::move(this->captured_piece);
    if (captured_piece) {
        board.game_state.pieces[captured_piece->get_color()].push_back(captured_piece);
        end_square.set_piece(std::move(captured_piece));
    }
}

bool Move::is_valid_rook_move(const Board& board) const {
    if (!is_same_line(this->start, this->end)) {
        return false;
    }
    if (!is_clear_line(this->start, this->end, board)) {
        return false;
    }
    return true;
}

bool Move::is_valid_bishop_move(const Board& board) const {
    if (!is_same_diagonal(this->start, this->end)) {
        return false;
    }
    if (!is_clear_diagonal(this->start, this->end, board)) {
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
    if (abs(end.x - start.x) == 1 && abs(end.y - start.y) == 2) {
        return true;
    }
    if (abs(end.x - start.x) == 2 && abs(end.y - start.y) == 1) {
        return true;
    }
    return false;
}

bool Move::is_valid_king_move() const {
    const int x_diff = abs(start.x - end.x);
    const int y_diff = abs(start.y - end.y);
    if (x_diff > 1 || y_diff > 1) {
        return false;
    }
    return true;
}

bool Move::is_valid_castling_move() const {
    const int king_x = 4;
    if (start.x != king_x) {
        return false;
    }
    if (end.x != 2 && end.x != 6) {
        return false;
    }

    return true;
}

bool Move::is_valid_pawn_move(const Board& board) const {
    const Square& start_square = board.get_square(start.x, start.y);
    const Square& end_square = board.get_square(end.x, end.y);
    
    const int direction = start_square.get_piece()->get_color() == BLACK ? 1 : -1;

    const int x_diff = end.x - start.x;
    const int y_diff = end.y - start.y;

    const bool end_square_is_empty = end_square.get_piece() == nullptr;
    const bool is_valid_move_one_square_forward = x_diff == 0 && y_diff == direction && end_square_is_empty;

    const int starting_row = start_square.get_piece()->get_color() == BLACK ? 1 : 6;
    const bool is_on_starting_row = start_square.get_y() == starting_row;
    const bool one_square_forward_is_empty = board.get_square(start.x, start.y + direction).get_piece() == nullptr;
    const bool is_valid_move_two_squares_forward = x_diff == 0 && y_diff == 2 * direction && is_on_starting_row && one_square_forward_is_empty && end_square_is_empty;

    const bool is_valid_capture = abs(x_diff) == 1 && y_diff == direction && !end_square_is_empty;

    if (is_valid_move_one_square_forward || is_valid_move_two_squares_forward || is_valid_capture) {
        return true;
    }
    return false;
}

bool Move::is_valid_pawn_move_threat(const Board& board) const {
    const int direction = board.get_square(start.x, start.y).get_piece()->get_color() == BLACK ? 1 : -1;
    const int x_diff = end.x - start.x;
    const int y_diff = end.y - start.y;

    const bool is_threatening_diagonally = abs(x_diff) == 1 && y_diff == direction;

    if (is_threatening_diagonally) {
        return true;
    }
    return false;
}

void Move::update_castling_rights(Board& board) const {
    const Color color = board.game_state.player_to_move;
    const int player_starting_row = color == WHITE ? 7 : 0; 
    const int opponent_starting_row = color == WHITE ? 0 : 7;
    const Pos player_king = Pos{4, player_starting_row};
    const Pos player_rook1 = Pos{0, player_starting_row};
    const Pos player_rook2 = Pos{7, player_starting_row};
    const Pos opponent_rook1 = Pos{0, opponent_starting_row};
    const Pos opponent_rook2 = Pos{7, opponent_starting_row};

    if (start == player_king) {
        board.game_state.castling_rights[color].kingside = false; 
        board.game_state.castling_rights[color].queenside = false; 
    } else if (start == player_rook1) {
        board.game_state.castling_rights[color].queenside = false; 
    } else if (start == player_rook2) {
        board.game_state.castling_rights[color].kingside = false; 
    } else if (end == opponent_rook1) {
        board.game_state.castling_rights[get_opposite_color(color)].queenside = false; 
    } else if (end == opponent_rook2) {
        board.game_state.castling_rights[get_opposite_color(color)].kingside = false; 
    }
}

Move Move::get_castling_rook_move(const Board& board) const {
    const int row = start.y;
    const int move_direction = end.x - start.x; 

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
    Move rook_move = Move(start_x, row, end_x, row);
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
    Square& start_square = board.get_square(start.x, start.y);
    Square& end_square = board.get_square(end.x, end.y);
    start_square.move_piece(end_square);

    const int x_diff = end.x - start.x;
    Square& captured_square = board.get_square(start.x + x_diff, start.y);
    board.remove_piece(captured_square.get_piece());
    this->captured_piece = captured_square.get_piece();
    captured_square.get_piece().reset();
}

void Move::undo_en_passant(Board& board) {
    Square& start_square = board.get_square(start.x, start.y);
    Square& end_square = board.get_square(end.x, end.y);
    end_square.move_piece(start_square);

    const int x_diff = end.x - start.x;
    Square& captured_square = board.get_square(start.x + x_diff, start.y);
    board.game_state.pieces[captured_piece->get_color()].push_back(captured_piece);
    captured_square.set_piece(this->captured_piece);
}

void Move::make_promotion(Board& board) {
    this->make(board);
    Square& end_square = board.get_square(end.x, end.y);
    if (!this->promotion_piece) {
        return;
    }
    std::shared_ptr<Piece> piece = end_square.get_piece();
    std::shared_ptr<Piece> promotion_piece = 
        create_piece(this->promotion_piece.value(), piece->get_color(), piece->get_x(), piece->get_y());
    end_square.set_piece(std::move(promotion_piece));
}

void Move::undo_promotion(Board& board) {
    this->undo(board);
    Square& start_square = board.get_square(start.x, start.y);
    std::shared_ptr<Piece> piece = start_square.get_piece();
    std::shared_ptr<Piece> pawn = std::make_shared<Pawn>(Pawn(piece->get_color(), piece->get_x(), piece->get_y()));
    start_square.set_piece(std::move(pawn));
}

void Move::make_pawn_two_squares_forward(Board& board) {
    this->make(board);
    board.game_state.en_passant_square = Pos{start.x, (end.y + start.y) / 2};
}

void Move::undo_pawn_two_squares_forward(Board& board) {
    this->undo(board);
    board.game_state.en_passant_square = {};
}
