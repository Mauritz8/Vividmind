#include "board_helper.h"

#include <iostream>
#include <stdexcept>

#include "board_utils.h"


BoardHelper::BoardHelper(Board& board)
    : board(board)
{}


bool BoardHelper::is_occupied_by_color(Pos pos, Color color) const {
    const Square& end = board.get_square(pos.x, pos.y);
    return end.piece && end.piece->color == color;
}

bool BoardHelper::is_clear_line(Pos pos1, Pos pos2) const {
    int x_direction = 0;
    int y_direction = 0;

    if (pos1.x != pos2.x) {
        x_direction = (pos2.x - pos1.x) > 0 ? 1 : -1;
    } else if (pos1.y != pos2.y) {
        y_direction = (pos2.y - pos1.y) > 0 ? 1 : -1;
    }

    int x = pos1.x + x_direction;
    int y = pos1.y + y_direction;
    while (x != pos2.x || y != pos2.y) {
        if (board.get_square(x, y).piece) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

bool BoardHelper::is_clear_diagonal(Pos pos1, Pos pos2) const {
    if (pos1 == pos2) {
        return true;
    }

    const int x_direction = (pos2.x - pos1.x) > 0 ? 1 : -1;
    const int y_direction = (pos2.y - pos1.y) > 0 ? 1 : -1;

    int x = pos1.x + x_direction;
    int y = pos1.y + y_direction;
    while (x != pos2.x && y != pos2.y) {
        if (board.get_square(x, y).piece) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

Pos BoardHelper::get_king_square(Color color) const {
    for (Piece piece : board.game_state.pieces[color]) {
        if (piece.piece_type == KING) {
            return piece.pos;
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}

void BoardHelper::make_appropriate(const Move& move) const {
    board.history.push_back(board.game_state);
    if (move.is_castling_move) {
        make_castling(move);
    } else if (move.is_en_passant) {
        make_en_passant(move);
    } else if (move.is_promotion) {
        make_promotion(move);
    } else if (move.is_pawn_two_squares_forward) {
        make_pawn_two_squares_forward(move);
    } else {
        make(move);
    }

    board.switch_player_to_move();
}

void BoardHelper::undo_appropriate(const Move& move) const {
    const Square& end_square = board.get_square(move.end);
    if (!end_square.piece) {
        std::cout << "Can't undo move\n"; 
        return;
    }

    if (move.is_castling_move) {
        undo_castling(move);
    } else if (move.is_en_passant) {
        undo_en_passant(move);
    } else if (move.is_promotion) {
        undo_promotion(move);
    } else if (move.is_pawn_two_squares_forward) {
        undo_pawn_two_squares_forward(move);
    } else {
        undo(move);
    }

    board.game_state = board.history.at(board.history.size() - 1);
    board.history.pop_back();
}

void BoardHelper::make(const Move& move) const {
    board.game_state.en_passant_square = {};
    board.game_state.captured_piece = {};
    Square& start_square = board.get_square(move.start); 
    Square& end_square = board.get_square(move.end); 
    std::optional<Piece> captured_piece = end_square.piece;
    if (captured_piece) {
        board.remove_piece(*captured_piece);
        board.game_state.material[captured_piece->color] -= captured_piece->get_value();
        board.game_state.psqt[captured_piece->color] -= captured_piece->get_psqt_score();
        board.game_state.captured_piece = captured_piece;
    }
    Piece& piece_in_game_state = board.get_piece(*start_square.piece);
    piece_in_game_state.pos = move.end;
    const int psqt_old = start_square.piece->get_psqt_score();
    board.move_piece(move.start, move.end);
    const int psqt_new = end_square.piece->get_psqt_score();
    board.game_state.psqt[end_square.piece->color] += psqt_new - psqt_old;

    update_castling_rights(move);
}

void BoardHelper::undo(const Move& move) const {
    Square& start_square = board.get_square(move.start); 
    Square& end_square = board.get_square(move.end); 
    board.move_piece(move.end, move.start);
    if (board.game_state.captured_piece) {
        end_square.piece = board.game_state.captured_piece;
    }
}

void BoardHelper::update_castling_rights(const Move& move) const {
    const Color color = board.game_state.player_to_move;
    const int player_starting_row = color == WHITE ? 7 : 0; 
    const int opponent_starting_row = color == WHITE ? 0 : 7;
    const Pos player_king = Pos{4, player_starting_row};
    const Pos player_rook1 = Pos{0, player_starting_row};
    const Pos player_rook2 = Pos{7, player_starting_row};
    const Pos opponent_rook1 = Pos{0, opponent_starting_row};
    const Pos opponent_rook2 = Pos{7, opponent_starting_row};

    if (move.start == player_king) {
        board.game_state.castling_rights[color].kingside = false; 
        board.game_state.castling_rights[color].queenside = false; 
    } else if (move.start == player_rook1) {
        board.game_state.castling_rights[color].queenside = false; 
    } else if (move.start == player_rook2) {
        board.game_state.castling_rights[color].kingside = false; 
    } else if (move.end == opponent_rook1) {
        board.game_state.castling_rights[get_opposite_color(color)].queenside = false; 
    } else if (move.end == opponent_rook2) {
        board.game_state.castling_rights[get_opposite_color(color)].kingside = false; 
    }
}

Move BoardHelper::get_castling_rook_move(const Move& move) const {
    const int row = move.start.y;
    const int move_direction = move.end.x - move.start.x; 

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

void BoardHelper::make_castling(const Move& move) const {
    make(move);
    Move rook_move = get_castling_rook_move(move);
    make(rook_move);
}

void BoardHelper::undo_castling(const Move& move) const {
    undo(move);
    Move rook_move = get_castling_rook_move(move);
    undo(rook_move);
}

void BoardHelper::make_en_passant(const Move& move) const {
    Square& start_square = board.get_square(move.start);
    Piece& piece_in_game_state = board.get_piece(*start_square.piece);
    piece_in_game_state.pos = move.end;
    board.move_piece(move.start, move.end);

    const int x_diff = move.end.x - move.start.x;
    Square& captured_square = board.get_square(move.start.x + x_diff, move.start.y);
    board.remove_piece(*captured_square.piece);
    board.game_state.captured_piece = captured_square.piece;
    board.game_state.material[captured_square.piece->color] -= captured_square.piece->get_value();
    board.game_state.psqt[captured_square.piece->color] -= captured_square.piece->get_psqt_score();
    captured_square.piece = {};
}

void BoardHelper::undo_en_passant(const Move& move) const {
    board.move_piece(move.end, move.start);

    const int x_diff = move.end.x - move.start.x;
    Square& captured_square = board.get_square(move.start.x + x_diff, move.start.y);
    board.game_state.pieces[board.game_state.captured_piece->color].push_back(*board.game_state.captured_piece);
    board.game_state.material[board.game_state.captured_piece->color] += board.game_state.captured_piece->get_value();
    captured_square.piece = board.game_state.captured_piece;
}

void BoardHelper::make_promotion(const Move& move) const {
    if (!move.promotion_piece) {
        return;
    }
    make(move);
    Square& end_square = board.get_square(move.end);
    Piece& piece_in_game_state = board.get_piece(*end_square.piece);
    piece_in_game_state.pos = move.end;
    piece_in_game_state.piece_type = *move.promotion_piece;
    const int old_psqt = end_square.piece->get_psqt_score();
    end_square.piece->piece_type = *move.promotion_piece;
    const int new_value = end_square.piece->get_value();
    const int new_psqt = end_square.piece->get_psqt_score();
    board.game_state.material[end_square.piece->color] += new_value - PAWN_VALUE;
    board.game_state.psqt[end_square.piece->color] += new_psqt - old_psqt;
}

void BoardHelper::undo_promotion(const Move& move) const {
    undo(move);
    Square& start_square = board.get_square(move.start);
    start_square.piece->piece_type = PAWN;
}

void BoardHelper::make_pawn_two_squares_forward(const Move& move) const {
    make(move);
    board.game_state.en_passant_square = Pos{move.start.x, (move.end.y + move.start.y) / 2};
}

void BoardHelper::undo_pawn_two_squares_forward(const Move& move) const {
    undo(move);
    board.game_state.en_passant_square = {};
}
