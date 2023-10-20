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
#include "game_state.h"
#include "piece.h"
#include "pieces/king.h"
#include "pieces/pawn.h"


Board Board::get_empty_board() {
    Board board;
    for (int i = 0; i < 8; i++) {
        std::vector<Square> row;
        for (int j = 0; j < 8; j++) {
            row.push_back(Square(j, i));
        }
        board.squares.push_back(row);
    }
    return board;
}

Board Board::get_starting_position() {
    return get_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board Board::get_position_from_fen(std::string fen) {
    Board board = Board::get_empty_board();
    
    std::istringstream ss(fen);
    std::array<std::string, 6> fen_parts;
    for (int i = 0; i < 6; i++) {
        std::getline(ss, fen_parts[i], ' ');
    }

    board.place_pieces(fen_parts[0]);
    board.set_player_to_move(fen_parts[1]);
    board.set_castling_rights(fen_parts[2]);
    board.set_en_passant_square(fen_parts[3]);
    return board;
}


const Square& Board::get_square(int x, int y) const { 
    if (x < 0 || x > 7 || y < 0 || y > 7) {
        throw std::invalid_argument("Square (" + std::to_string(x) + ", " + std::to_string(y) + ") is outside board");
    }
    return squares.at(y).at(x); 
}

Square& Board::get_square(int x, int y) { 
    if (x < 0 || x > 7 || y < 0 || y > 7) {
        throw std::invalid_argument("Square (" + std::to_string(x) + ", " + std::to_string(y) + ") is outside board");
    }
    return squares.at(y).at(x); 
}

void Board::set_square(int x, int y, std::shared_ptr<Piece> piece) {
    if (x < 0 || x > 7 || y < 0 || y > 7) {
        throw std::invalid_argument("Square (" + std::to_string(x) + ", " + std::to_string(y) + ") is outside board");
    }
    Square& square = squares.at(y).at(x); 
    square.piece = piece;
}


void Board::show() const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::shared_ptr<Piece>& piece = this->get_square(j, i).piece;
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
    for (auto piece : game_state.pieces[game_state.player_to_move]) {
        std::vector<Move> piece_moves = piece->get_psuedo_legal_moves(*this);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
}

std::vector<Move> Board::get_threatened_moves(Color color) {
    std::vector<Move> moves;
    for (auto piece : game_state.pieces[color]) {
        std::vector<Move> piece_moves = piece->get_threatened_moves(*this);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
}

void Board::remove_piece(std::shared_ptr<Piece> piece) {
    std::vector<std::shared_ptr<Piece>>& pieces = this->game_state.pieces[piece->color];
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (it->get() == piece.get()) {
            pieces.erase(it); 
            return;
        }
    }
}

void Board::place_pieces(const std::string& fen_piece_placement_field) {
    int index = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const char ch = fen_piece_placement_field.at(index++);
            const std::string pieces = "rnbqkp";
            if (pieces.find(tolower(ch)) != std::string::npos) {
                const Color color = islower(ch) ? BLACK : WHITE;
                std::shared_ptr<Piece> piece = std::make_shared<Piece>(Piece(get_piece_type(ch).value(), color, Pos{j, i}));
                this->game_state.pieces[color].push_back(piece);
                this->set_square(j, i, piece);
            } else if (ch >= '1' && ch <= '8') {
                const int num = ch - '0';
                j += num - 1;
            } else if (ch == '/') {
                j--;
            }
        }
    }
}

void Board::set_player_to_move(const std::string& fen_active_color_field) {
    if (fen_active_color_field.at(0) == 'w') {
        this->game_state.player_to_move = WHITE;
    } else if (fen_active_color_field.at(0) == 'b') {
        this->game_state.player_to_move = BLACK;
    } 
}

void Board::set_castling_rights(const std::string& fen_castling_field) {
    if (fen_castling_field.at(0) == '-') {
        this->game_state.castling_rights[WHITE].kingside = false;
        this->game_state.castling_rights[WHITE].queenside = false;
        this->game_state.castling_rights[BLACK].kingside = false;
        this->game_state.castling_rights[BLACK].queenside = false;
        return;
    }

    for (char ch : fen_castling_field) {
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

void Board::set_en_passant_square(const std::string& fen_en_passant_field) {
    if (fen_en_passant_field.size() != 2) {
        return;
    } 
    const int x = fen_en_passant_field[0] - 'a';
    const int y = 8 - (fen_en_passant_field[1] - '0');
    this->game_state.en_passant_square = Pos{x, y};
}
