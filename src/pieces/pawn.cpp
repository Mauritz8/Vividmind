#include "board.h"
#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/pawn.h"
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>


std::vector<Move> get_pawn_psuedo_legal_moves(Piece pawn, Board& board) {
    std::vector<Move> moves;
    const int direction = board.game_state.player_to_move == BLACK ? 1 : -1;

    Square start = Square(pawn.x, pawn.y);
    try {
        const Square& end1 = board.get_square(
                start.x,
                start.y + direction);
        if (!end1.piece) {
            moves.push_back(Move(start, end1));

            const Square& end2 = board.get_square(
                    start.x,
                    start.y + 2 * direction);
            const int starting_row = pawn.color == BLACK ? 1 : 6;
            if (start.y == starting_row && !end2.piece) {
                Move move = Move(start, end2);
                move.is_pawn_two_squares_forward = true;
                moves.push_back(move);
            }
        }
    } catch (const std::invalid_argument& e) {}


    std::vector<Move> captures = get_pawn_captures(pawn, board);
    for (Move& capture : captures) {
        const Square& end = board.get_square(capture.end.x, capture.end.y);
        if (end.piece) {
            moves.push_back(capture);
        } else if (is_valid_en_passant(capture, board)) {
            capture.is_en_passant = true;
            moves.push_back(capture);
        }
    }

    std::vector<Move> other_promotion_moves;
    const std::array<Piece_type, 3> other_promotion_piece_types = {
        ROOK,
        BISHOP,
        KNIGHT
    };
    for (Move& move : moves) {
        if (is_promotion_move(move)) {
            move.is_promotion = true;
            move.promotion_piece = QUEEN;
            for (Piece_type piece_type : other_promotion_piece_types) {
                Move promotion_move = move;
                promotion_move.promotion_piece = piece_type;
                other_promotion_moves.push_back(promotion_move);
            }
        }
    }
    moves.insert(moves.end(), other_promotion_moves.begin(), other_promotion_moves.end());

    return moves;
}

std::vector<Move> get_pawn_captures(Piece pawn, const Board& board) {
    const int direction = pawn.color == BLACK ? 1 : -1;

    const Pos start = Pos{pawn.x, pawn.y};
    const Pos end1 = Pos{pawn.x + 1, pawn.y + direction};
    const Pos end2 = Pos{pawn.x - 1, pawn.y + direction};

    std::vector<Move> moves;
    if (!is_outside_board(end1)) {
        moves.push_back(Move(start, end1));
    }
    if (!is_outside_board(end2)) {
        moves.push_back(Move(start, end2));
    }
    return moves;
}

bool is_valid_en_passant(const Move& pawn_capture, const Board& board) {
    if (!board.game_state.en_passant_square.has_value()) {
        return false;
    }
    return pawn_capture.end == board.game_state.en_passant_square.value();
}

bool is_promotion_move(const Move& pawn_move) {
    if (pawn_move.end.y == 0 || pawn_move.end.y == 7) {
        return true;
    }
    return false;
}
