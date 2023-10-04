#include <algorithm>
#include <iostream>
#include <memory>
#include <cctype>
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
    Board board = get_empty_board();
    board.set_player_to_move(WHITE);

    board.setup_pieces(BLACK);
    board.setup_pawns(BLACK);
    board.setup_pieces(WHITE);
    board.setup_pawns(WHITE);
    return board;
}

Board Board::get_position_from_fen(std::string fen) {
    Board board = Board::get_empty_board();

    const std::string piece_placement_field = fen.substr(0, fen.find(" "));
    fen.erase(0, piece_placement_field.length() + 1);

    const std::string active_color_field = fen.substr(0, fen.find(" "));
    fen.erase(0, active_color_field.length() + 1);

    const std::string castling_availability_field = fen.substr(0, fen.find(" "));
    fen.erase(0, castling_availability_field.length() + 1);

    const std::string en_passant_target_square_field = fen.substr(0, fen.find(" "));
    fen.erase(0, en_passant_target_square_field.length() + 1);

    const std::string halfmove_clock_field = fen.substr(0, fen.find(" "));
    fen.erase(0, halfmove_clock_field.length() + 1);

    const std::string fullmove_number_field = fen.substr(0, fen.find(" "));
    fen.erase(0, fullmove_number_field.length() + 1);

    board.place_pieces(piece_placement_field);
    board.set_player_to_move(active_color_field);

    return board;
}


const Square& Board::get_square(int x, int y) const { 
    return squares.at(y).at(x); 
}

Square& Board::get_square(int x, int y) { 
    return squares.at(y).at(x); 
}

void Board::set_square(int x, int y, std::unique_ptr<Piece> piece) {
    Square& square = squares.at(y).at(x); 
    square.set_piece(std::move(piece));
}

Color Board::get_player_to_move() const { 
    return player_to_move; 
}

void Board::set_player_to_move(Color player_to_move) { 
    this->player_to_move = player_to_move; 
}


void Board::show() const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::unique_ptr<Piece>& piece = this->get_square(j, i).get_piece();
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
    if (this->get_player_to_move() == WHITE) {
        this->set_player_to_move(BLACK);
    } else {
        this->set_player_to_move(WHITE);
    }
}

void Board::setup_pieces(Color color) {
    const int row = color == WHITE ? 7 : 0;

    auto p1 = std::make_unique<Rook>(Rook(color, 0, row));
    this->set_square(0, row, std::move(p1));
    auto p2 = std::make_unique<Knight>(Knight(color, 1, row));
    this->set_square(1, row, std::move(p2));
    auto p3 = std::make_unique<Bishop>(Bishop(color, 2, row));
    this->set_square(2, row, std::move(p3));
    auto p4 = std::make_unique<Queen>(Queen(color, 3, row));
    this->set_square(3, row, std::move(p4));
    auto p5 = std::make_unique<King>(King(color, 4, row));
    this->set_square(4, row, std::move(p5));
    auto p6 = std::make_unique<Bishop>(Bishop(color, 5, row));
    this->set_square(5, row, std::move(p6));
    auto p7 = std::make_unique<Knight>(Knight(color, 6, row));
    this->set_square(6, row, std::move(p7));
    auto p8 = std::make_unique<Rook>(Rook(color, 7, row));
    this->set_square(7, row, std::move(p8));
}

void Board::setup_pawns(Color color) {
    const int row = color == WHITE ? 6 : 1;
    for (int i = 0; i < 8; i++) {
        this->set_square(i, row, std::make_unique<Pawn>(Pawn(color, i, row)));
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
                std::unique_ptr<Piece> piece;
                switch (get_piece_type(ch).value()) {
                    case PAWN: {
                        piece = std::make_unique<Pawn>(Pawn(color, j, i));
                        break;
                    }
                    case KNIGHT: {
                        piece = std::make_unique<Knight>(Knight(color, j, i));
                        break;
                    }
                    case BISHOP: {
                        piece = std::make_unique<Bishop>(Bishop(color, j, i));
                        break;
                    }
                    case ROOK: {
                        piece = std::make_unique<Rook>(Rook(color, j, i));
                        break;
                    }
                    case QUEEN: {
                        piece = std::make_unique<Queen>(Queen(color, j, i));
                        break;
                    }
                    case KING: {
                        piece = std::make_unique<King>(King(color, j, i));
                        break;
                    }
                }
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
        this->set_player_to_move(WHITE);
    } else if (fen_active_color_field.at(0) == 'b') {
        this->set_player_to_move(BLACK);
    } 
}
