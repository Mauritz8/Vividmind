#include <iostream>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <string>

#include "board.h"
#include "piece.h"


static void setup_pieces(Board& board, const int row, const Piece_type* order, const Color color) {
    for (int i = 0; i < 8; i++) {
        std::optional piece{Piece(order[i], color)};
        board.get_square(i, row).set_piece(piece);
    }
}

static void setup_pawns(Board& board, const int row, const Color color) {
    for (int i = 0; i < 8; i++) {
        std::optional piece{Piece(PAWN, color)};
        board.get_square(i, row).set_piece(piece);
    }
}

void setup_board(Board& board) {
    board.set_player_to_move(WHITE);

    // setup square coordinates
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board.get_square(j, i).set_x(j);
            board.get_square(j, i).set_y(i);
        }
    }

    // setup pieces
    const Piece_type PIECE_ORDER[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    setup_pieces(board, 0, PIECE_ORDER, BLACK);
    setup_pawns(board, 1, BLACK);
    setup_pieces(board, 7, PIECE_ORDER, WHITE);
    setup_pawns(board, 6, WHITE);
    
    // set the piece as null for all squares that don't have a piece
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            board.get_square(j, i).set_piece({});
        }
    }
}

void show_board(Board& board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::optional<Piece> piece = board.get_square(j, i).get_piece();
            if (piece.has_value()) {
                printf(" %c", piece.value().get_char_representation());
            } else {
                printf(" _");
            }
        }
        printf("\n");
    }
    printf("\n");
}


/*void deallocate_board(Board& board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board.get_square(j, i).get_piece(); 
            if (piece) {
                free(piece);
            }
        }
    }
}*/

/*Board copy_board(Board& board) {
    Board board_copy;
    board_copy.set_player_to_move(board.get_player_to_move());
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square square = {square.x = j, square.y = i};
            const Piece* piece = board.get_square(j, i).piece;
            if (piece) {
                square.piece = malloc(sizeof(Piece));
                *square.piece = *board.get_square(j, i).piece;
            } else {
                square.piece = NULL;
            }
            board_copy.squares[i][j] = square;
        }
    }
    return board_copy;
}*/

static int place_pieces(const char* fen_piece_placement_field, Board& board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square square = board.get_square(j, i);
            square.set_x(j);
            square.set_y(i);
            char ch = *fen_piece_placement_field++;
            std::string pieces = "rnbqkp";
            if (pieces.find(tolower(ch)) != std::string::npos) {
                Color color;
                if (islower(ch)) {
                    color = BLACK;
                } else {
                    color = WHITE;
                }
                Piece piece(Piece::get_piece_type(ch), color);
            } else if (ch >= '1' && ch <= '8') {
                int num = ch - '0';
                for (int k = 0; k < num; k++) {
                    int x = j + k;
                    board.get_square(x, i).set_x(x);
                    board.get_square(x, i).set_y(i);
                    board.get_square(x, i).set_piece({});
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

static int set_player_to_move(const char* fen_active_color_field, Board& board) {
    if (*fen_active_color_field == 'w') {
        board.set_player_to_move(WHITE);
    } else if (*fen_active_color_field == 'b') {
        board.set_player_to_move(BLACK);
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

Board* get_position_from_fen(char* fen) {
    Board* board = malloc(sizeof(Board));
    const char* piece_placement_field = strtok(fen, " ");
    const char* active_color_field = strtok(NULL, " ");
    const char* castling_availability_field = strtok(NULL, " ");
    const char* en_passant_target_square_field = strtok(NULL, " ");
    const char* halfmove_clock_field = strtok(NULL, " ");
    const char* fullmove_number_field = strtok(NULL, " ");

    const int place_pieces_status = place_pieces(piece_placement_field, board);
    const int set_player_to_move_status = set_player_to_move(active_color_field, board);

    if (place_pieces_status == EXIT_FAILURE || set_player_to_move_status == EXIT_FAILURE) {
        return NULL;
    }
    return board;
}
