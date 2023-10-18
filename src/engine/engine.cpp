#include <functional>
#include <iostream>
#include <limits.h>
#include <memory>
#include <optional>
#include <vector>

#include "board.h"
#include "engine/engine.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/bishop.h"
#include "pieces/king.h"
#include "pieces/knight.h"
#include "pieces/pawn.h"
#include "pieces/queen.h"
#include "pieces/rook.h"


static int get_material_score(Color color, const Board& board) {
    int material = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::shared_ptr<Piece>& piece = board.get_square(j, i).get_piece();
            if (piece && piece->get_color() == color) {
                material += piece->get_value();
            }
        }
    }
    return material;
}

static int get_piece_square_table_score(Color color, const Board& board) {
    const auto pieces = get_all_pieces(color, board);
    const bool is_black = color == BLACK;

    int score = 0;
    for (const std::shared_ptr<Piece>& piece : pieces) {
        int value;
        if (is_black) {
            value = piece->get_piece_square_table().at(7 - piece->get_y()).at(piece->get_x()); 
        } else {
            value = piece->get_piece_square_table().at(piece->get_y()).at(piece->get_x()); 
        }
        score += value;
    }

    return score;
}

static double evaluate(const Board& board) {
    double score = 0;
    const int white_material = get_material_score(WHITE, board);
    const int black_material = get_material_score(BLACK, board);
    score += white_material - black_material;

    const double white_piece_square_table_score = get_piece_square_table_score(WHITE, board);
    const double black_piece_square_table_score = get_piece_square_table_score(BLACK, board);
    score += 0.01 * (white_piece_square_table_score - black_piece_square_table_score);

    if (board.player_to_move == BLACK) {
        return -score;
    }
    return score;
}

static double nega_max(int depth, Board& board, std::vector<Move>& move_history) {
    if (depth == 0) {
        return evaluate(board);
    }
    double max = INT_MIN;
    std::vector<Move> legal_moves = get_all_legal_moves(board, move_history);
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate(board, move_history);
        const double score = -nega_max(depth - 1, board, move_history);
        move.undo_appropriate(board, move_history);
        if (score > max) {
            max = score;
        }
    }
    return max;
}

Move get_best_move(int depth, const Board& board, const std::vector<Move>& move_history) {
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;

    double max = INT_MIN;
    const std::vector<Move> legal_moves = get_all_legal_moves(board_copy, move_history_copy);
    const Move* best_move = nullptr;
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate(board_copy, move_history_copy);
        const double score = -nega_max(depth - 1, board_copy, move_history_copy);
        move.undo_appropriate(board_copy, move_history_copy);
        if (score > max) {
            max = score;
            best_move = &legal_moves.at(i);
        }
    }
    return *best_move;
}


int perft(int depth, Board board, std::vector<Move> move_history) {
    if (depth == 0) {
        return 1;
    }

    int nodes = 0;
    const std::vector<Move> move_list = get_all_legal_moves(board, move_history);
    for (int i = 0; i < move_list.size(); i++) {
        Move move = move_list.at(i);
        move.make_appropriate(board, move_history);
        nodes += perft(depth - 1, board, move_history);    
        move.undo_appropriate(board, move_history);
    }
    return nodes;
}

void divide(int depth, Board board, std::vector<Move> move_history) {
    std::cout << "";
    int nodes_searched = 0;
    const std::vector<Move> move_list = get_all_legal_moves(board, move_history);
    for (int i = 0; i < move_list.size(); i++) {
        Move move = move_list.at(i);
        move.make_appropriate(board, move_history);
        const int nodes = perft(depth - 1, board, move_history);
        nodes_searched += nodes;
        std::cout << move.to_uci_notation() << ": " << nodes << "\n";
        move.undo_appropriate(board, move_history);
    }
    std::cout << "\nNodes searched: " << nodes_searched << "\n";
}
