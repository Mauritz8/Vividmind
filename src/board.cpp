#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <cctype>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "board.h"
#include "board_utils.h"
#include "game_state.h"
#include "piece.h"
#include "pieces/king.h"
#include "pieces/pawn.h"


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


const Square& Board::get_square(int x, int y) const { 
    if (is_outside_board(x, y)) {
        throw std::invalid_argument("Square (" + std::to_string(x) + ", " + std::to_string(y) + ") is outside board");
    }
    return squares.at(y).at(x); 
}

Square& Board::get_square(int x, int y) { 
    if (is_outside_board(x, y)) {
        throw std::invalid_argument("Square (" + std::to_string(x) + ", " + std::to_string(y) + ") is outside board");
    }
    return squares.at(y).at(x); 
}

const Square& Board::get_square(Pos pos) const {
    return get_square(pos.x, pos.y);
}

Square& Board::get_square(Pos pos) {
    return get_square(pos.x, pos.y);
}


void Board::show() const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::optional<Piece>& piece = this->get_square(j, i).piece;
            if (piece) {
                std::cout << " " << piece->get_char_representation();
            } else {
                std::cout << " _";
            }
        }
        std::cout << "\n";
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


std::vector<Move> Board::get_psuedo_legal_moves() {
    std::vector<Move> moves;
    for (Piece piece : game_state.pieces[game_state.player_to_move]) {
        std::vector<Move> piece_moves = piece.get_psuedo_legal_moves(*this);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
}

std::vector<Move> Board::get_threatened_moves(Color color) {
    std::vector<Move> moves;
    for (Piece piece : game_state.pieces[color]) {
        std::vector<Move> piece_moves = piece.get_threatened_moves(*this);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
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

void Board::move_piece(Pos from, Pos to) {
    Square& start = get_square(from);
    Square& end = get_square(to);
    start.piece->pos = to;
    end.piece = start.piece;
    start.piece = {};
}

void Board::place_pieces(const std::string& pieces) {
    int x = 0;
    int y = 0;
    for (const char ch : pieces) {
        if (isdigit(ch)) {
            const int n = (int) ch - '0';
            for (int i = 0; i < n; i++) {
                Square& square = get_square(x, y);
                square.x = x;
                square.y = y;
                x++;
            }
        } else if (ch == '/') {
            y++;        
            x = 0;
        } else {
            Color color = islower(ch) ? BLACK : WHITE;
            Piece_type piece_type = *get_piece_type(ch);
            Piece piece = Piece(piece_type, color, Pos{x, y});
            Square& square = get_square(x, y);
            square.x = x;
            square.y = y;
            square.piece = piece;
            game_state.pieces[color].push_back(piece);
            game_state.material[color] += piece.get_value();                
            game_state.psqt[color] += piece.get_psqt_score();
            x++;
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
    this->game_state.en_passant_square = Pos{x, y};
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
