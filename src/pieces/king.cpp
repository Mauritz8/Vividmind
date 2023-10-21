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

std::vector<Move> get_castling_moves(Board& board) {
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

    std::vector<Move> threatened_moves = board.get_threatened_moves(opponent);
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


std::vector<Move> get_king_threatened_moves(Piece king, const Board& board) {
    const std::array<Pos, 8> end_squares = {
        Pos{king.pos.x, king.pos.y + 1},
        Pos{king.pos.x + 1, king.pos.y + 1},
        Pos{king.pos.x + 1, king.pos.y},
        Pos{king.pos.x + 1, king.pos.y - 1},
        Pos{king.pos.x, king.pos.y - 1},
        Pos{king.pos.x - 1, king.pos.y - 1},
        Pos{king.pos.x - 1, king.pos.y},
        Pos{king.pos.x - 1, king.pos.y + 1}
    };

    std::vector<Move> moves;
    for (Pos end : end_squares) {
        if (!is_outside_board(end) && !is_occupied_by_color(end, king.color, board)) {
            moves.push_back(Move(king.pos, end));
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

