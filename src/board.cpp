#include <array>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <string>
#include <vector>

#include "board.h"
#include "piece.h"


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
    std::vector<Piece_type> PIECE_ORDER = 
        {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    board.setup_pieces(0, PIECE_ORDER, BLACK);
    board.setup_pawns(1, BLACK);
    board.setup_pieces(7, PIECE_ORDER, WHITE);
    board.setup_pawns(6, WHITE);
    return board;
}

Board Board::get_position_from_fen(std::string fen) {
    Board board = Board::get_empty_board();
    const std::string piece_placement_field = fen.substr(0, fen.find(" "));
    const std::string active_color_field =
        fen.substr(piece_placement_field.length(), fen.find(" "));
    const std::string castling_availability_field =
        fen.substr(piece_placement_field.length() +
                active_color_field.length(), fen.find(" "));
    const std::string en_passant_target_square_field =
        fen.substr(piece_placement_field.length() +
                active_color_field.length() +
                castling_availability_field.length(), fen.find(" "));
    const std::string halfmove_clock_field =
        fen.substr(piece_placement_field.length() +
                active_color_field.length() +
                castling_availability_field.length() +
                en_passant_target_square_field.length(), fen.find(" "));
    const std::string fullmove_number_field =
        fen.substr(piece_placement_field.length() +
                active_color_field.length() +
                castling_availability_field.length() +
                en_passant_target_square_field.length() +
                halfmove_clock_field.length(), fen.find(" "));

    const int place_pieces_status = board.place_pieces(piece_placement_field);
    const int set_player_to_move_status = board.set_player_to_move(active_color_field);

    return board;
}


const Square& Board::get_square(int x, int y) const { 
    return squares.at(y).at(x); 
}

Square& Board::get_square(int x, int y) { 
    return squares.at(y).at(x); 
}

void Board::set_square(int x, int y, const std::optional<Piece> piece) {
    Square& square = squares.at(y).at(x); 
    square.set_piece(piece);
}

Color Board::get_player_to_move() const { 
    return player_to_move; 
}

void Board::set_player_to_move(Color player_to_move) { 
    this->player_to_move = player_to_move; 
}


void Board::show_board() const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::optional<Piece> piece = this->get_square(j, i).get_piece();
            if (piece.has_value()) {
                std::cout << " " << get_char_representation(piece.value().get_piece_type());
            } else {
                std::cout << " _";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}


void Board::setup_pieces(int row, std::vector<Piece_type> order, Color color) {
    for (int i = 0; i < order.size(); i++) {
        Piece piece = Piece(order[i], color);
        this->set_square(i, row, piece);
    }
}

void Board::setup_pawns(int row, Color color) {
    for (int i = 0; i < 8; i++) {
        Piece piece = Piece(PAWN, color);
        this->set_square(i, row, piece);
    }
}

int Board::place_pieces(const std::string fen_piece_placement_field) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square square = this->get_square(j, i);
            char ch = fen_piece_placement_field.at(i + j);
            std::string pieces = "rnbqkp";
            if (pieces.find(tolower(ch)) != std::string::npos) {
                Color color;
                if (islower(ch)) {
                    color = BLACK;
                } else {
                    color = WHITE;
                }
                Piece piece(get_piece_type(ch).value(), color);
            } else if (ch >= '1' && ch <= '8') {
                int num = ch - '0';
                j += num - 1;
            } else if (ch == '/') {
                j--;
                continue;
            } else {
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

int Board::set_player_to_move(const std::string fen_active_color_field) {
    if (fen_active_color_field.at(0) == 'w') {
        this->set_player_to_move(WHITE);
    } else if (fen_active_color_field.at(0) == 'b') {
        this->set_player_to_move(BLACK);
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
