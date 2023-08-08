#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "piece.h"

static void setup_pieces(Board* board, const int row, const Piece_type* order, const Color color) {
    for (int i = 0; i < 8; i++) {
        board->squares[row][i].piece = malloc(sizeof(Piece));
        board->squares[row][i].piece->piece_type = order[i];
        board->squares[row][i].piece->color = color;
    }
}

static void setup_pawns(Board* board, const int row, const Color color) {
    for (int i = 0; i < 8; i++) {
        board->squares[row][i].piece = malloc(sizeof(Piece));
        board->squares[row][i].piece->piece_type = PAWN;
        board->squares[row][i].piece->color = color;
    }
}

void setup_board(Board* board) {
    board->player_to_move = WHITE;

    // setup square coordinates
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->squares[i][j].x = j;
            board->squares[i][j].y = i;
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
            board->squares[i][j].piece = NULL;
        }
    }
}

void show_board(const Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const Piece* piece = board->squares[i][j].piece;
            if (piece) {
                printf(" %c", get_char_representation(piece->piece_type));
            } else {
                printf(" _");
            }
        }
        printf("\n");
    }
    printf("\n");
}


void deallocate_board(Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->squares[i][j].piece; 
            if (piece) {
                free(piece);
            }
        }
    }
}

Board copy_board(const Board* board) {
    Board board_copy;
    board_copy.player_to_move = board->player_to_move;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square square = {square.x = j, square.y = i};
            const Piece* piece = board->squares[i][j].piece;
            if (piece) {
                square.piece = malloc(sizeof(Piece));
                *square.piece = *board->squares[i][j].piece;
            } else {
                square.piece = NULL;
            }
            board_copy.squares[i][j] = square;
        }
    }
    return board_copy;
}

static int place_pieces(const char* fen_piece_placement_field, Board* board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->squares[i][j].x = j;
            board->squares[i][j].y = i;
            char ch = *fen_piece_placement_field++;
            if (strchr("rnbqkp", tolower(ch))) {
                board->squares[i][j].piece = malloc(sizeof(Piece));
                board->squares[i][j].piece->piece_type = get_piece_type(ch);
                if (islower(ch)) {
                    board->squares[i][j].piece->color = BLACK;
                } else {
                    board->squares[i][j].piece->color = WHITE;
                }
            } else if (ch >= '1' && ch <= '8') {
                int num = ch - '0';
                for (int k = 0; k < num; k++) {
                    board->squares[i][j + k].piece = NULL;
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

static int set_player_to_move(const char* fen_active_color_field, Board* board) {
    if (*fen_active_color_field == 'w') {
        board->player_to_move = WHITE;
    } else if (*fen_active_color_field == 'b') {
        board->player_to_move = BLACK;
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