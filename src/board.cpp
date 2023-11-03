#include "board.hpp"

#include <iostream>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "board_utils.hpp"
#include "psqt.hpp"


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
    return board;
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

Piece& Board::get_piece(Piece piece) {
    for (Piece& p : game_state.pieces[piece.color]) {
        if (p.pos == piece.pos) {
            return p;
        } 
    }
    throw std::invalid_argument("could not find piece");
}

void Board::remove_piece(Piece piece) {
    std::vector<Piece>& pieces = this->game_state.pieces[piece.color];
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (*it == piece) {
            pieces.erase(it); 
            return;
        }
    }
}

void Board::move_piece(int from, int to) {
    Square& start = squares[from];
    Square& end = squares[to];
    start.piece->pos = to;
    end.piece = start.piece;
    start.piece = {};
}

bool Board::is_endgame() const {
    std::array<Color, 2> colors = {WHITE, BLACK};
    for (Color color : colors) {
        for (const Piece& piece : game_state.pieces[color]) {
            if (piece.piece_type == QUEEN) {
                return false;
            } 
        }
    }
    return true;
}

int Board::get_psqt_score(const Piece& piece) const {
    const int x = piece.pos % 8;
    const int y = piece.pos / 8;
    const int square = piece.color == WHITE ? piece.pos : x + (7 - y) * 8;
    switch (piece.piece_type) {
        case KING: {
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
            game_state.pieces[color].push_back(piece);
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

bool operator==(GameState state1, GameState state2) {
    for (int i = 0; i < 2; i++) {
        std::vector<Piece> pieces1 = state1.pieces[i];
        std::vector<Piece> pieces2 = state2.pieces[i];
        if (pieces1.size() != pieces2.size()) {
            return false;
        }

        for (int j = 0; j < state1.pieces[i].size(); j++) {
            if (pieces1[j] != pieces2[j]) {
                return false;
            }
        }
    }
    return true;
}
