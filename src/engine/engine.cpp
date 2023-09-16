#include <limits.h>
#include <optional>
#include <vector>

#include "board.h"
#include "engine/engine.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"


static PieceCounts get_piece_counts(const Board& board, Color color) {
    PieceCounts piece_counts = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const std::optional<Piece>& piece = board.get_square(j, i).get_piece();
            if (!piece.has_value() || piece.value().get_color() != color) {
                continue;
            }
            switch (piece->get_piece_type()) {
                case KING:
                    piece_counts.kings++;
                    break;
                case QUEEN:
                    piece_counts.queens++;
                    break;
                case ROOK:
                    piece_counts.rooks++;
                    break;
                case BISHOP:
                    piece_counts.bishops++;
                    break;
                case KNIGHT:
                    piece_counts.knights++;
                    break;
                case PAWN:
                    piece_counts.pawns++;
                    break;
            }
        }
    }
    return piece_counts;
}

static int get_material_score(const Board& board) {
    const PieceCounts player_piece_counts = get_piece_counts(board, board.get_player_to_move());

    const Color opponent_color = get_opposite_color(board.get_player_to_move());
    const PieceCounts opponent_piece_counts = get_piece_counts(board, opponent_color);

    return KING_SCORE * (player_piece_counts.kings - opponent_piece_counts.kings) +
           QUEEN_SCORE * (player_piece_counts.queens - opponent_piece_counts.queens) +
           ROOK_SCORE * (player_piece_counts.rooks - opponent_piece_counts.rooks) +
           BISHOP_SCORE * (player_piece_counts.bishops - opponent_piece_counts.bishops) +
           KNIGHT_SCORE * (player_piece_counts.knights - opponent_piece_counts.knights) +
           PAWN_SCORE * (player_piece_counts.pawns - opponent_piece_counts.pawns);
}

static int evaluate(const Board& board) {
    const int material_score = get_material_score(board);
    const int score = material_score;
    return score;
}

static int nega_max(int depth, Board& board, std::vector<Move>& move_history) {
    if (depth == 0) {
        return evaluate(board);
    }
    int max = INT_MIN;
    std::vector<Move> legal_moves = get_all_legal_moves(board, move_history);
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate_move(board, move_history);
        const int score = -nega_max(depth - 1, board, move_history);
        move.undo_appropriate_move(board, move_history);
        if (score > max) {
            max = score;
        }
    }
    return max;
}

Move get_best_move(int depth, const Board& board, const std::vector<Move>& move_history) {
    Board board_copy = board;
    std::vector<Move> move_history_copy = move_history;

    int max = INT_MIN;
    Move best_move;
    const std::vector<Move> legal_moves = get_all_legal_moves(board_copy, move_history_copy);
    for (int i = 0; i < legal_moves.size(); i++) {
        Move move = legal_moves.at(i);
        move.make_appropriate_move(board_copy, move_history_copy);
        const int score = -nega_max(depth, board_copy, move_history_copy);
        move.undo_appropriate_move(board_copy, move_history_copy);
        if (score > max) {
            max = score;
            best_move = legal_moves.at(i);
        }
    }
    return best_move;
}
