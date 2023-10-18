#include <algorithm>
#include <iostream>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#include "board.h"
#include "piece.h"
#include "pieces/bishop.h"
#include "pieces/king.h"
#include "pieces/knight.h"
#include "pieces/pawn.h"
#include "pieces/queen.h"
#include "pieces/rook.h"


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

    const std::string pieces_field = fen.substr(0, fen.find(" "));
    fen.erase(0, pieces_field.length() + 1);

    const std::string active_color_field = fen.substr(0, fen.find(" "));
    fen.erase(0, active_color_field.length() + 1);

    const std::string castling_field = fen.substr(0, fen.find(" "));
    fen.erase(0, castling_field.length() + 1);

    const std::string en_passant_field = fen.substr(0, fen.find(" "));
    fen.erase(0, en_passant_field.length() + 1);

    const std::string halfmove_clock_field = fen.substr(0, fen.find(" "));
    fen.erase(0, halfmove_clock_field.length() + 1);

    const std::string fullmove_number_field = fen.substr(0, fen.find(" "));
    fen.erase(0, fullmove_number_field.length() + 1);

    board.place_pieces(pieces_field);
    board.set_player_to_move(active_color_field);
    board.set_castling_rights(castling_field);

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
    square.set_piece(piece);
}


void Board::show() const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::shared_ptr<Piece>& piece = this->get_square(j, i).get_piece();
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
    if (this->player_to_move == WHITE) {
        this->player_to_move = BLACK;
    } else {
        this->player_to_move = WHITE;
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
                std::shared_ptr<Piece> piece = create_piece(get_piece_type(ch).value(), color, j, i);
                this->set_square(j, i, std::move(piece));
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
        this->player_to_move = WHITE;
    } else if (fen_active_color_field.at(0) == 'b') {
        this->player_to_move = BLACK;
    } 
}

void Board::set_castling_rights(const std::string& fen_castling_field) {
    if (fen_castling_field.at(0) == '-') {
        this->castling_rights.white_kingside = false;
        this->castling_rights.white_queenside = false;
        this->castling_rights.black_kingside = false;
        this->castling_rights.black_queenside = false;
        return;
    }

    for (char ch : fen_castling_field) {
        switch (ch) {
            case 'K':
                this->castling_rights.white_kingside = true;
                break;
            case 'Q':
                this->castling_rights.white_queenside = true;
                break;
            case 'k':
                this->castling_rights.black_kingside = true;
                break;
            case 'q':
                this->castling_rights.black_queenside = true;
                break;
        }
    }
}
