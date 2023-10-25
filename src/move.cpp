#include "move.h"

#include <iostream>

#include "board_helper.h"
#include "board_utils.h"
#include "move_validator.h"


Move::Move(int start_x, int start_y, int end_x, int end_y) {
    this->start = {start_x, start_y};
    this->end = {end_x, end_y};
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move::Move(Pos start, Pos end) {
    this->start = start;
    this->end = end;
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move::Move(const Square& start, const Square& end) {
    this->start = Pos{start.x, start.y};
    this->end = Pos{end.x, end.y};
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

    BoardHelper board_helper = BoardHelper(board);
    MoveValidator move_validator = MoveValidator(board, board_helper);
    if (start.piece->piece_type == KING && move_validator.is_valid_castling_move(move)) {
        move.is_castling_move = true;
    } else if (start.piece->piece_type == PAWN) {
        if (move_validator.is_valid_en_passant(move)) {
            move.is_en_passant = true;
        } else if (move_validator.is_promotion_move(move)) {
            move.is_promotion = true;
            const char promotion_piece = uci_notation[4];
            move.promotion_piece = get_piece_type(promotion_piece);
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
    this->is_pawn_two_squares_forward = move.is_pawn_two_squares_forward;
}


Move Move::operator=(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->is_castling_move = move.is_castling_move;
    this->is_promotion = move.is_promotion;
    this->promotion_piece = move.promotion_piece;
    this->is_en_passant = move.is_en_passant;
    this->is_pawn_two_squares_forward = move.is_pawn_two_squares_forward;
    return *this;
}

void Move::make_appropriate(Board& board) const {
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

void Move::undo_appropriate(Board& board) const {
    const Square& end_square = board.get_square(end);
    if (!end_square.piece) {
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

    if (this->promotion_piece) {
        uci_notation += tolower(get_char_representation(this->promotion_piece.value()));
    }
    return uci_notation;
}

void Move::make(Board& board) const {
    board.game_state.en_passant_square = {};
    board.game_state.captured_piece = {};
    Square& start_square = board.get_square(start); 
    Square& end_square = board.get_square(end); 
    std::optional<Piece> captured_piece = end_square.piece;
    if (captured_piece) {
        board.remove_piece(*captured_piece);
        board.game_state.material[captured_piece->color] -= captured_piece->get_value();
        board.game_state.psqt[captured_piece->color] -= captured_piece->get_psqt_score();
        board.game_state.captured_piece = captured_piece;
    }
    Piece& piece_in_game_state = board.get_piece(*start_square.piece);
    piece_in_game_state.pos = end;
    const int psqt_old = start_square.piece->get_psqt_score();
    board.move_piece(start, end);
    const int psqt_new = end_square.piece->get_psqt_score();
    board.game_state.psqt[end_square.piece->color] += psqt_new - psqt_old;

    this->update_castling_rights(board);
}

void Move::undo(Board& board) const {
    Square& start_square = board.get_square(start); 
    Square& end_square = board.get_square(end); 
    board.move_piece(end, start);
    if (board.game_state.captured_piece) {
        end_square.piece = board.game_state.captured_piece;
    }
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

void Move::make_castling(Board& board) const {
    this->make(board);
    Move rook_move = this->get_castling_rook_move(board);
    rook_move.make(board);
}

void Move::undo_castling(Board& board) const {
    this->undo(board);
    Move rook_move = this->get_castling_rook_move(board);
    rook_move.undo(board);
}

void Move::make_en_passant(Board& board) const {
    Square& start_square = board.get_square(start);
    Piece& piece_in_game_state = board.get_piece(*start_square.piece);
    piece_in_game_state.pos = end;
    board.move_piece(start, end);

    const int x_diff = end.x - start.x;
    Square& captured_square = board.get_square(start.x + x_diff, start.y);
    board.remove_piece(*captured_square.piece);
    board.game_state.captured_piece = captured_square.piece;
    board.game_state.material[captured_square.piece->color] -= captured_square.piece->get_value();
    board.game_state.psqt[captured_square.piece->color] -= captured_square.piece->get_psqt_score();
    captured_square.piece = {};
}

void Move::undo_en_passant(Board& board) const {
    board.move_piece(end, start);

    const int x_diff = end.x - start.x;
    Square& captured_square = board.get_square(start.x + x_diff, start.y);
    board.game_state.pieces[board.game_state.captured_piece->color].push_back(*board.game_state.captured_piece);
    board.game_state.material[board.game_state.captured_piece->color] += board.game_state.captured_piece->get_value();
    captured_square.piece = board.game_state.captured_piece;
}

void Move::make_promotion(Board& board) const {
    if (!this->promotion_piece) {
        return;
    }
    this->make(board);
    Square& end_square = board.get_square(end);
    Piece& piece_in_game_state = board.get_piece(*end_square.piece);
    piece_in_game_state.pos = end;
    piece_in_game_state.piece_type = *promotion_piece;
    const int old_psqt = end_square.piece->get_psqt_score();
    end_square.piece->piece_type = promotion_piece.value();
    const int new_value = end_square.piece->get_value();
    const int new_psqt = end_square.piece->get_psqt_score();
    board.game_state.material[end_square.piece->color] += new_value - PAWN_VALUE;
    board.game_state.psqt[end_square.piece->color] += new_psqt - old_psqt;
}

void Move::undo_promotion(Board& board) const {
    this->undo(board);
    Square& start_square = board.get_square(start);
    start_square.piece->piece_type = PAWN;
}

void Move::make_pawn_two_squares_forward(Board& board) const {
    this->make(board);
    board.game_state.en_passant_square = Pos{start.x, (end.y + start.y) / 2};
}

void Move::undo_pawn_two_squares_forward(Board& board) const {
    this->undo(board);
    board.game_state.en_passant_square = {};
}
