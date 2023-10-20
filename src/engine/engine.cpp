#include <functional>
#include <iostream>
#include <limits.h>
#include <memory>
#include <optional>
#include <vector>

#include "board.h"
#include "engine/engine.h"
#include "game_over_conditions.h"
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
    for (auto piece : board.game_state.pieces[color]) {
        material += piece->get_value();
    }
    return material;
}

static int get_piece_square_table_score(Color color, const std::vector<std::shared_ptr<Piece>>& pieces) {
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

    const double white_piece_square_table_score = get_piece_square_table_score(WHITE, board.game_state.pieces[WHITE]);
    const double black_piece_square_table_score = get_piece_square_table_score(BLACK, board.game_state.pieces[BLACK]);
    score += 0.01 * (white_piece_square_table_score - black_piece_square_table_score);

    if (board.game_state.player_to_move == BLACK) {
        return -score;
    }
    return score;
}

static double nega_max(int depth, Board& board) {
    if (is_checkmate(board)) {
        return -200;
    }
    if (is_draw(board)) {
        return 0;
    }
    if (depth == 0) {
        return evaluate(board);
    }
    double max = INT_MIN;
    std::vector<Move> legal_moves = get_legal_moves(board);
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate(board);
        const double score = -nega_max(depth - 1, board);
        move.undo_appropriate(board);
        if (score > max) {
            max = score;
        }
    }
    return max;
}

Move get_best_move(int depth, Board& board) {

    double max = INT_MIN;
    const std::vector<Move> legal_moves = get_legal_moves(board);
    const Move* best_move = nullptr;
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate(board);
        const double score = -nega_max(depth - 1, board);
        move.undo_appropriate(board);
        if (score > max) {
            max = score;
            best_move = &legal_moves.at(i);
        }
    }
    std::cout << "eval = " << max << "\n";
    return *best_move;
}


int perft(int depth, Board board) {
    if (depth == 0) {
        return 1;
    }

    int nodes = 0;
    const std::vector<Move> move_list = get_legal_moves(board);
    for (int i = 0; i < move_list.size(); i++) {
        Move move = move_list.at(i);
        move.make_appropriate(board);
        nodes += perft(depth - 1, board);    
        move.undo_appropriate(board);
    }
    return nodes;
}

void divide(int depth, Board board) {
    std::cout << "";
    int nodes_searched = 0;
    const std::vector<Move> move_list = get_legal_moves(board);
    for (int i = 0; i < move_list.size(); i++) {
        Move move = move_list.at(i);
        move.make_appropriate(board);
        const int nodes = perft(depth - 1, board);
        nodes_searched += nodes;
        std::cout << move.to_uci_notation() << ": " << nodes << "\n";
        move.undo_appropriate(board);
    }
    std::cout << "\nNodes searched: " << nodes_searched << "\n";
}
