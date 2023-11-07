#include "board.hpp"

#include <array>
#include <iostream>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "piece.hpp"
#include "utils.hpp"
#include "engine/psqt.hpp"


const std::array<int, 120> Board::mailbox = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
    -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
    -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
    -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
    -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
    -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
    -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const std::array<int, 64> Board::mailbox64 = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98
};

Board Board::get_starting_position() {
    return get_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board Board::get_position_from_fen(std::string fen) {
    std::istringstream ss(fen);
    std::array<std::string, 6> fen_parts;
    for (int i = 0; i < 6; i++) {
        std::getline(ss, fen_parts[i], ' ');
    }

    Board board;
    board.game_state.material[WHITE] = 0;
    board.game_state.material[BLACK] = 0;
    board.game_state.psqt[WHITE] = 0;
    board.game_state.psqt[BLACK] = 0;
    board.place_pieces(fen_parts[0]);
    board.set_player_to_move(fen_parts[1]);
    board.set_castling_rights(fen_parts[2]);
    board.set_en_passant_square(fen_parts[3]);
    board.game_state.halfmove_clock = std::stoi(fen_parts[4]);
    board.game_state.fullmove_number =  std::stoi(fen_parts[5]);
    return board;
}


bool Board::operator==(const Board& other) const {
    for (int pos = 0; pos < 64; pos++) {
        if (squares[pos].piece != other.squares[pos].piece) {
            return false;
        } 
    }

    if (game_state.en_passant_square != other.game_state.en_passant_square) {
        return false;
    }

    std::array<Color, 2> colors = {WHITE, BLACK};
    for (Color color : colors) {
        if (game_state.castling_rights[color].kingside != other.game_state.castling_rights[color].kingside) {
            return false;
        }
        if (game_state.castling_rights[color].queenside != other.game_state.castling_rights[color].queenside) {
            return false;
        }
    }

    return true;
}

void Board::show() const {
    for (int i = 0; i < 64; i++) {
        const std::optional<Piece>& piece = this->squares[i].piece;
        if (piece) {
            std::cout << " " << piece->get_char_representation();
        } else {
            std::cout << " _";
        }

        if ((i + 1) % 8 == 0) {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void Board::switch_player_to_move() {
    if (this->game_state.player_to_move == WHITE) {
        this->game_state.player_to_move = BLACK;
    } else {
        this->game_state.player_to_move = WHITE;
    }
}

void Board::make(const Move& move) {
    game_state.next_move = move;
    history.push_back(game_state);

    game_state.en_passant_square = {};
    game_state.captured_piece = {};

    Square& start = squares[move.start]; 
    Square& end = squares[move.end]; 

    if (start.piece->piece_type == PAWN) {
        game_state.halfmove_clock = 0;

        if (move.is_pawn_two_squares_forward) {
            const int direction = start.piece->color == BLACK ? 1 : -1;
            game_state.en_passant_square = move.start + direction * 8;
        } else if (move.is_en_passant) {
            const int x_diff = move.end % 8 - move.start % 8;
            Square& captured_square = squares[move.start + x_diff];
            remove_piece(*captured_square.piece);
            captured_square.piece = {};
        } else if (move.is_promotion) {
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

    if (move.is_castling_move) {
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

    if (move.is_castling_move) {
        Move rook_move = get_castling_rook_move(move);
        move_piece(squares[rook_move.end], squares[rook_move.start]);
    } else if (move.is_en_passant) {
        Piece captured_piece = *game_state.captured_piece;
        Square& captured_square = squares[captured_piece.pos];
        captured_square.piece = captured_piece;
        pieces[captured_piece.color].push_back(captured_piece);
        game_state.captured_piece = {};
    } else if (move.is_promotion) {
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

int Board::get_king_square(Color color) const {
    for (Piece piece : pieces[color]) {
        if (piece.piece_type == KING) {
            return piece.pos;
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}

bool Board::is_insufficient_material() const {
    for (const std::vector<Piece>& pieces : pieces) {
        for (Piece piece : pieces) {
            if (piece.piece_type == PAWN || piece.piece_type == ROOK || piece.piece_type == QUEEN) {
                return false;
            }
        }
    }
    return true;
}

// this will only check if the same position repeats twice
// because otherwise it will do one repetition in a lot of positions,
// before it realizes that it has to do something else
bool Board::is_threefold_repetition() const {
    Board old_board = *this;
    const int history_size = history.size();
    for (int _ = 0; _ < history_size - 1; _++) {

        // if there has been a capture or a pawn move
        // it's impossible to reach the same position again 
        if (old_board.game_state.halfmove_clock == 0) {
            return false;
        }

        old_board.undo();
        if (*this == old_board) {
            return true;
        }         
    }
    return false;
}

bool Board::is_draw_by_fifty_move_rule() const {
    return game_state.halfmove_clock > 100;
}

void Board::place_pieces(const std::string& pieces) {
    int pos = 0;
    for (const char ch : pieces) {
        if (ch == '/') continue;

        if (isdigit(ch)) {
            const int n = (int) ch - '0';
            for (int i = 0; i < n; i++) {
                squares[pos].pos = pos;
                pos++;
            }
        } else {
            Color color = islower(ch) ? BLACK : WHITE;
            Piece piece = Piece(get_piece_type(ch), color, pos);
            Square& square = squares[pos];
            squares[pos].pos = pos;
            squares[pos].piece = piece;
            this->pieces[color].push_back(piece);
            game_state.material[color] += piece.get_value();                
            game_state.psqt[color] += get_psqt_score(piece);
            pos++;
        }
    }
}

void Board::set_player_to_move(const std::string& player_to_move) {
    if (player_to_move == "w") {
        this->game_state.player_to_move = WHITE;
    } else if (player_to_move == "b") {
        this->game_state.player_to_move = BLACK;
    } 
}

void Board::set_castling_rights(const std::string& castling_rights) {
    this->game_state.castling_rights[WHITE].kingside = false;
    this->game_state.castling_rights[WHITE].queenside = false;
    this->game_state.castling_rights[BLACK].kingside = false;
    this->game_state.castling_rights[BLACK].queenside = false;

    for (char ch : castling_rights) {
        switch (ch) {
            case 'K':
                this->game_state.castling_rights[WHITE].kingside = true;
                break;
            case 'Q':
                this->game_state.castling_rights[WHITE].queenside = true;
                break;
            case 'k':
                this->game_state.castling_rights[BLACK].kingside = true;
                break;
            case 'q':
                this->game_state.castling_rights[BLACK].queenside = true;
                break;
        }
    }
}

void Board::set_en_passant_square(const std::string& en_passant_square) {
    if (en_passant_square.size() != 2) {
        return;
    } 
    const int x = en_passant_square[0] - 'a';
    const int y = 8 - (en_passant_square[1] - '0');
    this->game_state.en_passant_square = x + y * 8;
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

int Board::get_psqt_score(const Piece& piece) const {
    const int x = piece.pos % 8;
    const int y = piece.pos / 8;
    const int square = piece.color == WHITE ? piece.pos : x + (7 - y) * 8;
    switch (piece.piece_type) {
        case KING: {
            if (is_lone_king(piece.color)) {
                return KING_MATE[square];
            }
            if (is_endgame()) {
                return KING_ENDGAME_PSQT[square];
            }
            return KING_PSQT[square];
        }
        case QUEEN: return QUEEN_PSQT[square];
        case ROOK: return ROOK_PSQT[square];
        case BISHOP: return BISHOP_PSQT[square];
        case KNIGHT: return KNIGHT_PSQT[square];
        case PAWN: return PAWN_PSQT[square];
    }
}

bool Board::is_lone_king(Color color) const {
    if (pieces[color].size() == 1) {
        return true;
    }
    return false;
}

bool Board::is_endgame() const {
    return game_state.material[WHITE] - KING_VALUE < 1500 && 
           game_state.material[BLACK] - KING_VALUE < 1500;
}
