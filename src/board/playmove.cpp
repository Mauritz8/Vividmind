#include "board.hpp"

#include <stdexcept>

#include "utils.hpp"


void Board::make(const Move& move) {
    game_state.next_move = move;
    history.push_back(game_state);

    game_state.en_passant_square = {};
    game_state.captured_piece = {};

    Square& start = squares[move.start]; 
    Square& end = squares[move.end]; 

    if (start.piece->piece_type == PAWN) {
        game_state.halfmove_clock = 0;

        if (move.move_type == PAWN_TWO_SQUARES_FORWARD) {
            const int direction = start.piece->color == BLACK ? 1 : -1;
            game_state.en_passant_square = move.start + direction * 8;
        } else if (move.move_type == EN_PASSANT) {
            const int x_diff = move.end % 8 - move.start % 8;
            Square& captured_square = squares[move.start + x_diff];
            remove_piece(*captured_square.piece);
            captured_square.piece = {};
        } else if (move.move_type == PROMOTION) {
            if (end.piece) {
                remove_piece(*end.piece);
            }

            Piece& piece = *start.piece;
            Piece& piece_in_game_state = get_piece(piece);
            PieceType promotion_piece = *move.promotion_piece;

            const int old_psqt = get_psqt_score(piece);
            piece.piece_type = promotion_piece;
            piece_in_game_state.piece_type = promotion_piece;
            const int new_value = piece.get_value();
            const int new_psqt = get_psqt_score(piece);
            game_state.material[piece.color] += new_value - PAWN_VALUE;
            game_state.psqt[piece.color] += new_psqt - old_psqt;
        } 
    } 

    if (move.move_type == CASTLING) {
        Move rook_move = get_castling_rook_move(move);
        move_piece(squares[rook_move.start], squares[rook_move.end]);
    } else {
        if (end.piece) {
            game_state.halfmove_clock = 0;
            remove_piece(*end.piece);
        } else {
            game_state.halfmove_clock++;
        }
    }

    move_piece(start, end);

    update_castling_rights(move);

    // increment the fullmove number after every time black moves
    if (game_state.player_to_move == BLACK) {
        game_state.fullmove_number++;
    }
    switch_player_to_move();
}

void Board::undo() {
    Move move = history.back().next_move;
    Square& start = squares[move.start]; 
    Square& end = squares[move.end]; 

    if (move.move_type == CASTLING) {
        Move rook_move = get_castling_rook_move(move);
        move_piece(squares[rook_move.end], squares[rook_move.start]);
    } else if (move.move_type == EN_PASSANT) {
        Piece captured_piece = *game_state.captured_piece;
        Square& captured_square = squares[captured_piece.pos];
        captured_square.piece = captured_piece;
        pieces[captured_piece.color].push_back(captured_piece);
        game_state.captured_piece = {};
    } else if (move.move_type == PROMOTION) {
        end.piece->piece_type = PAWN;
        Piece& piece_in_piece_list = get_piece(*end.piece);
        piece_in_piece_list.piece_type = PAWN;
    } 

    move_piece(end, start);
    if (game_state.captured_piece) {
        end.piece = game_state.captured_piece;
        pieces[game_state.captured_piece->color].push_back(*game_state.captured_piece);
    }

    game_state = history.at(history.size() - 1);
    history.pop_back();
}

Move Board::get_castling_rook_move(const Move& move) const {
    const int row = move.start / 8;
    const int move_direction = move.end - move.start; 

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
    Move rook_move = Move(start_x + row * 8, end_x +  row * 8);
    return rook_move;
}


Piece& Board::get_piece(Piece piece) {
    for (Piece& p : pieces[piece.color]) {
        if (p.pos == piece.pos) {
            return p;
        } 
    }
    throw std::invalid_argument("could not find piece");
}

void Board::remove_piece(Piece piece) {
    std::vector<Piece>& pieces = this->pieces[piece.color];
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (*it == piece) {
            pieces.erase(it); 
            game_state.material[piece.color] -= piece.get_value();
            game_state.psqt[piece.color] -= get_psqt_score(piece);
            game_state.captured_piece = piece;
            return;
        }
    }
}

void Board::move_piece(Square& start, Square& end) {
    Piece& piece_in_game_state = get_piece(*start.piece);
    piece_in_game_state.pos = end.pos;


    const int psqt_old = get_psqt_score(*start.piece);

    start.piece->pos = end.pos;
    end.piece = start.piece;
    start.piece = {};

    const int psqt_new = get_psqt_score(*end.piece);
    game_state.psqt[end.piece->color] += psqt_new - psqt_old;
}

void Board::update_castling_rights(const Move& move) {
    const Color color = game_state.player_to_move;
    const int player_starting_row = color == WHITE ? 7 : 0; 
    const int opponent_starting_row = color == WHITE ? 0 : 7;
    const int player_king = player_starting_row * 8 + 4;
    const int player_rook1 = player_starting_row * 8 + 0;
    const int player_rook2 = player_starting_row * 8 + 7;
    const int opponent_rook1 = opponent_starting_row * 8 + 0;
    const int opponent_rook2 = opponent_starting_row * 8 + 7;

    if (move.start == player_king) {
        game_state.castling_rights[color].kingside = false; 
        game_state.castling_rights[color].queenside = false; 
    } else if (move.start == player_rook1) {
        game_state.castling_rights[color].queenside = false; 
    } else if (move.start == player_rook2) {
        game_state.castling_rights[color].kingside = false; 
    } 

    if (move.end == opponent_rook1) {
        game_state.castling_rights[get_opposite_color(color)].queenside = false; 
    } else if (move.end == opponent_rook2) {
        game_state.castling_rights[get_opposite_color(color)].kingside = false; 
    }
}
