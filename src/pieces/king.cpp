#include "board.h"
#include "board_utils.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/king.h"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>


static std::vector<Move> get_potential_castling_moves(const Board& board) {
    const Color color = board.game_state.player_to_move;
    const int king_x = 4;
    const int row = color == WHITE ? 7 : 0;
    const int kingside_end_x = 6;
    const int queenside_end_x = 2;

    std::vector<Move> potential_castling_moves;
    if (board.game_state.castling_rights[color].kingside) {
        potential_castling_moves.push_back(Move(king_x, row, kingside_end_x, row));
    } 

    if (board.game_state.castling_rights[color].queenside) {
        potential_castling_moves.push_back(Move(king_x, row, queenside_end_x, row));
    } 

    return potential_castling_moves;
}

static std::vector<Move> get_castling_moves(Board& board) {
    std::vector<Move> castling_moves = get_potential_castling_moves(board);
    for (auto it = castling_moves.begin(); it != castling_moves.end();) {
        if (!is_valid_castling(*it, board)) {
            it = castling_moves.erase(it);
        } else {
            it->is_castling_move = true;
            ++it;
        }
    }
    return castling_moves;
}


static bool passes_through_check_when_castling(const Move& castling_move, Board& board) {
    const int row = castling_move.start.y;
    const int start_x = castling_move.start.x;
    const int end_x = castling_move.end.x;
    const int direction = end_x - start_x > 0 ? 1 : -1;
    const Color opponent = get_opposite_color(board.game_state.player_to_move);

    std::vector<Move> threatened_moves = ::get_threatened_moves(opponent, board);
    for (Move& move : threatened_moves) {
        int x = start_x;
        while (x != end_x) {
            if (move.end == Pos{x + direction, row}) {
                return true;
            }
            x += direction;
        }
    }
    return false;
}


std::vector<Move> get_king_psuedo_legal_moves(const Square& start, Board& board) {
    std::vector<Move> moves = get_king_threatened_moves(start, board);
    std::vector<Move> castling_moves = get_castling_moves(board);
    moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
    return moves;
}

std::vector<Move> get_king_threatened_moves(const Square& start, const Board& board) {
    const std::array<Pos, 8> end_squares = {
        Pos{start.x, start.y + 1},
        Pos{start.x + 1, start.y + 1},
        Pos{start.x + 1, start.y},
        Pos{start.x + 1, start.y - 1},
        Pos{start.x, start.y - 1},
        Pos{start.x - 1, start.y - 1},
        Pos{start.x - 1, start.y},
        Pos{start.x - 1, start.y + 1}
    };

    std::vector<Move> moves;
    for (Pos end: end_squares) {
        if (!is_outside_board(end)) {
            moves.push_back(Move(start.x, start.y, end.x, end.y));
        }
    }
    return moves;
}

bool is_valid_castling(const Move& move, Board& board) {
    int rook_x;
    if (move.end.x == 6) {
        rook_x = 7;
    } else if (move.end.x == 2) {
        rook_x = 0;
    } 
    
    const Pos king_pos = Pos{move.start.x, move.start.y};
    const Pos rook_pos = Pos{rook_x, move.start.y};
    if (!is_clear_line(king_pos, rook_pos, board)) {
        return false;
    }
    if (is_in_check(board.game_state.player_to_move, board)) {
        return false;
    }
    if (passes_through_check_when_castling(move, board)) {
        return false;
    }

    return true;
}

