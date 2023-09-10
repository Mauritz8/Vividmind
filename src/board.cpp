#include <iostream>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <string>

#include "board.h"
#include "piece.h"


void Board::setup_pieces(const int row, const Piece_type* order, const Color color) {
    for (int i = 0; i < 8; i++) {
        std::optional piece{Piece(order[i], color)};
        this->get_square(i, row).set_piece(piece);
    }
}

void Board::setup_pawns(int row, Color color) {
    for (int i = 0; i < 8; i++) {
        std::optional piece{Piece(PAWN, color)};
        this->get_square(i, row).set_piece(piece);
    }
}

void Board::setup_board() {
    this->set_player_to_move(WHITE);

    // setup square coordinates
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            this->get_square(j, i).set_x(j);
            this->get_square(j, i).set_y(i);
        }
    }

    // setup pieces
    const Piece_type PIECE_ORDER[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    this->setup_pieces(0, PIECE_ORDER, BLACK);
    this->setup_pawns(1, BLACK);
    this->setup_pieces(7, PIECE_ORDER, WHITE);
    this->setup_pawns(6, WHITE);
    
    // set the piece as null for all squares that don't have a piece
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            this->get_square(j, i).set_piece({});
        }
    }
}

void Board::show_board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::optional<Piece> piece = this->get_square(j, i).get_piece();
            if (piece.has_value()) {
                printf(" %c", get_char_representation(piece.value().get_piece_type()));
            } else {
                printf(" _");
            }
        }
        printf("\n");
    }
    printf("\n");
}

int Board::place_pieces(const std::string fen_piece_placement_field) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square square = this->get_square(j, i);
            square.set_x(j);
            square.set_y(i);
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
                for (int k = 0; k < num; k++) {
                    int x = j + k;
                    this->get_square(x, i) = Square(x, i);
                }
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

Board::Board(std::string fen) {
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

    const int place_pieces_status = this->place_pieces(piece_placement_field);
    const int set_player_to_move_status = this->set_player_to_move(active_color_field);
};
