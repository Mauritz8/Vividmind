#include <iostream>
#include <optional>
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

void Board::set_square(int x, int y, const std::optional<Piece>& piece) {
    Square& square = squares.at(y).at(x); 
    square.set_piece(piece);
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
            const std::optional<Piece>& piece = this->get_square(j, i).get_piece();
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

void Board::switch_player_to_move() {
    if (this->get_player_to_move() == WHITE) {
        this->set_player_to_move(BLACK);
    } else {
        this->set_player_to_move(WHITE);
    }
}

void Board::setup_pieces(int row, const std::vector<Piece_type> order, Color color) {
    for (int i = 0; i < order.size(); i++) {
        const Piece piece = Piece(order.at(i), color);
        this->set_square(i, row, piece);
    }
}

void Board::setup_pawns(int row, Color color) {
    for (int i = 0; i < 8; i++) {
        const Piece piece = Piece(PAWN, color);
        this->set_square(i, row, piece);
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
                const Piece piece = Piece(get_piece_type(ch).value(), color);
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
        this->set_player_to_move(WHITE);
    } else if (fen_active_color_field.at(0) == 'b') {
        this->set_player_to_move(BLACK);
    } 
}
