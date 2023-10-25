#include "move_generator.h"

#include <array>
#include <iostream>
#include <stdexcept>

#include "board_utils.h"
#include "pos.h"
#include "square.h"


MoveGenerator::MoveGenerator(Board& board)
    : board(board)
    , board_helper(board)
    , move_validator(board, board_helper)
{}

std::vector<Move> MoveGenerator::get_legal_moves(bool only_captures) const {
    std::vector<Move> legal_moves = get_psuedo_legal_moves(only_captures);
    for (auto it = legal_moves.begin(); it != legal_moves.end();) {
        if (leaves_king_in_check(*it)) {
            it = legal_moves.erase(it);
        } else {
            ++it;
        }
    }
    return legal_moves;
}

std::vector<Move> MoveGenerator::get_threatened_moves(Color color) const {
    std::vector<Move> moves;
    for (Piece piece : board.game_state.pieces[color]) {
        std::vector<Move> piece_moves = get_threatened_moves(piece);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
}

int MoveGenerator::perft(int depth) const {
    if (depth == 0) {
        return 1;
    }

    int nodes = 0;
    std::vector<Move> move_list = get_legal_moves(false);
    for (Move& move : move_list) {
        move.make_appropriate(board);
        nodes += perft(depth - 1);    
        move.undo_appropriate(board);
    }
    return nodes;
}

void MoveGenerator::divide(int depth) const {
    std::cout << "";
    int nodes_searched = 0;
    std::vector<Move> move_list = get_legal_moves(false);
    for (Move& move : move_list) {
        move.make_appropriate(board);
        const int nodes = perft(depth - 1);
        nodes_searched += nodes;
        std::cout << move.to_uci_notation() << ": " << nodes << "\n";
        move.undo_appropriate(board);
    }
    std::cout << "\nNodes searched: " << nodes_searched << "\n";
}

bool MoveGenerator::leaves_king_in_check(const Move& move) const {
    const Color player_to_move = board.game_state.player_to_move;
    move.make_appropriate(board);
    if (is_in_check(player_to_move)) {
        move.undo_appropriate(board);
        return true;
    }
    move.undo_appropriate(board);
    return false;
}

bool MoveGenerator::is_in_check(Color color) const {
    const Pos king_square = board_helper.get_king_square(color);
    const Color opponent = get_opposite_color(color);
    const std::vector<Move> psuedo_legal_moves = get_threatened_moves(opponent);
    for (const Move& move : psuedo_legal_moves) {
        if (move.end == king_square) {
            return true;
        }
    }
    return false;
}

std::vector<Move> MoveGenerator::get_psuedo_legal_moves(bool only_captures) const {
    std::vector<Move> moves;
    for (Piece piece : board.game_state.pieces[board.game_state.player_to_move]) {
        std::vector<Move> piece_moves = get_psuedo_legal_moves(piece, only_captures);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
}

std::vector<Move> MoveGenerator::get_psuedo_legal_moves(const Piece& piece, bool only_captures) const {
    switch (piece.piece_type) {
        case KING: {
            return get_king_psuedo_legal_moves(piece, only_captures);
        }
        case QUEEN: {
            return get_queen_psuedo_legal_moves(piece, only_captures);
        }
        case ROOK: {
            return get_rook_psuedo_legal_moves(piece, only_captures);
        }
        case BISHOP: {
            return get_bishop_psuedo_legal_moves(piece, only_captures);
        }
        case KNIGHT: {
            return get_knight_psuedo_legal_moves(piece, only_captures);
        }
        case PAWN: {
            return get_pawn_psuedo_legal_moves(piece, only_captures);
        }
    }    
    return {};
}

std::vector<Move> MoveGenerator::get_threatened_moves(const Piece& piece) const {
    if (piece.piece_type == PAWN) {
        return get_pawn_captures(piece);
    }
    if (piece.piece_type == KING) {
        return get_king_threatened_moves(piece, false);
    }
    return get_psuedo_legal_moves(piece, false);
}


std::vector<Move> MoveGenerator::get_bishop_psuedo_legal_moves(const Piece& piece, bool only_captures) const {
    std::vector<Move> moves;

    std::vector<Move> up_right = get_psuedo_legal_moves_direction(piece, 1, 1, only_captures);
    std::vector<Move> down_right = get_psuedo_legal_moves_direction(piece, 1, -1, only_captures);
    std::vector<Move> down_left = get_psuedo_legal_moves_direction(piece, -1, -1, only_captures);
    std::vector<Move> up_left = get_psuedo_legal_moves_direction(piece, -1, 1, only_captures);

    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));

    return moves;
}

std::vector<Move> MoveGenerator::get_rook_psuedo_legal_moves(const Piece& piece, bool only_captures) const {
    std::vector<Move> moves;

    std::vector<Move> up = get_psuedo_legal_moves_direction(piece, 0, 1, only_captures);
    std::vector<Move> right = get_psuedo_legal_moves_direction(piece, 1, 0, only_captures);
    std::vector<Move> down = get_psuedo_legal_moves_direction(piece, 0, -1, only_captures);
    std::vector<Move> left = get_psuedo_legal_moves_direction(piece, -1, 0, only_captures);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(left), std::end(left));

    return moves;
}

std::vector<Move> MoveGenerator::get_queen_psuedo_legal_moves(const Piece& piece, bool only_captures) const {
    std::vector<Move> moves;

    std::vector<Move> up = get_psuedo_legal_moves_direction(piece, 0, 1, only_captures);
    std::vector<Move> up_right = get_psuedo_legal_moves_direction(piece, 1, 1, only_captures);
    std::vector<Move> right = get_psuedo_legal_moves_direction(piece, 1, 0, only_captures);
    std::vector<Move> down_right = get_psuedo_legal_moves_direction(piece, 1, -1, only_captures);
    std::vector<Move> down = get_psuedo_legal_moves_direction(piece, 0, -1, only_captures);
    std::vector<Move> down_left = get_psuedo_legal_moves_direction(piece, -1, -1, only_captures);
    std::vector<Move> left = get_psuedo_legal_moves_direction(piece, -1, 0, only_captures);
    std::vector<Move> up_left = get_psuedo_legal_moves_direction(piece, -1, 1, only_captures);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(left), std::end(left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));


    return moves;
}

std::vector<Move> MoveGenerator::get_knight_psuedo_legal_moves(const Piece& piece, bool only_captures) const {
    Pos start = piece.pos;
    const std::array<Pos, 8> end_squares = {
        Pos{start.x + 1, start.y + 2},
        Pos{start.x + 1, start.y - 2},
        Pos{start.x - 1, start.y + 2},
        Pos{start.x - 1, start.y - 2},
        Pos{start.x + 2, start.y + 1},
        Pos{start.x + 2, start.y - 1},
        Pos{start.x - 2, start.y + 1},
        Pos{start.x - 2, start.y - 1}
    };

    std::vector<Move> moves;
    for (Pos end : end_squares) {
        if (!is_outside_board(end)) {
            if (only_captures && board_helper.is_occupied_by_color(end, get_opposite_color(piece.color))) {
                moves.push_back(Move(start, end));
            } else if (!only_captures && !board_helper.is_occupied_by_color(end, piece.color)) {
                moves.push_back(Move(start, end));
            }
        }
    }
    return moves;
}

std::vector<Move> MoveGenerator::get_king_psuedo_legal_moves(const Piece& piece, bool only_captures) const {
    std::vector<Move> moves = get_king_threatened_moves(piece, only_captures);
    if (!only_captures) {
        std::vector<Move> castling_moves = get_castling_moves();
        moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
    }
    return moves;
}

std::vector<Move> MoveGenerator::get_pawn_psuedo_legal_moves(Piece pawn, bool only_captures) const {
    std::vector<Move> moves;
    const int direction = board.game_state.player_to_move == BLACK ? 1 : -1;

    Pos start = pawn.pos;

    if (!only_captures) {
        try {
            const Square& end1 = board.get_square(
                    start.x,
                    start.y + direction);
            if (!end1.piece) {
                moves.push_back(Move(start.x, start.y, end1.x, end1.y));

                const Square& end2 = board.get_square(
                        start.x,
                        start.y + 2 * direction);
                const int starting_row = pawn.color == BLACK ? 1 : 6;
                if (start.y == starting_row && !end2.piece) {
                    Move move = Move(start.x, start.y, end2.x, end2.y);
                    move.is_pawn_two_squares_forward = true;
                    moves.push_back(move);
                }
            }
        } catch (const std::invalid_argument& e) {}
    }


    std::vector<Move> captures = get_pawn_captures(pawn);
    for (Move& capture : captures) {
        const Square& end = board.get_square(capture.end);
        if (end.piece && end.piece->color != pawn.color) {
            moves.push_back(capture);
        } else if (move_validator.is_valid_en_passant(capture)) {
            capture.is_en_passant = true;
            moves.push_back(capture);
        }
    }

    std::vector<Move> other_promotion_moves;
    const std::array<PieceType, 3> other_promotion_piece_types = {
        ROOK,
        BISHOP,
        KNIGHT
    };
    for (Move& move : moves) {
        if (move_validator.is_promotion_move(move)) {
            move.is_promotion = true;
            move.promotion_piece = QUEEN;
            for (PieceType piece_type : other_promotion_piece_types) {
                Move promotion_move = move;
                promotion_move.promotion_piece = piece_type;
                other_promotion_moves.push_back(promotion_move);
            }
        }
    }
    moves.insert(moves.end(), other_promotion_moves.begin(), other_promotion_moves.end());

    return moves;
}

std::vector<Move> MoveGenerator::get_psuedo_legal_moves_direction(const Piece& piece, int x_direction, int y_direction, bool only_captures) const {
    std::vector<Move> moves;

    int x = piece.pos.x + x_direction;
    int y = piece.pos.y + y_direction;
    while (!is_outside_board(x, y)) {
        const Square& end = board.get_square(x, y);
        if (end.piece) {
            if (end.piece->color != piece.color) {
                moves.push_back(Move(piece.pos, Pos{x, y}));
            }
            break;
        }

        if (!only_captures) {
            moves.push_back(Move(piece.pos, Pos{x, y}));
        }
        x += x_direction;
        y += y_direction;
    }

    return moves;
}

std::vector<Move> MoveGenerator::get_king_threatened_moves(Piece king, bool only_captures) const {
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
        if (!is_outside_board(end) ) {
            if (only_captures && board_helper.is_occupied_by_color(end, get_opposite_color(king.color))) {
                moves.push_back(Move(king.pos, end));
            } else if (!only_captures && !board_helper.is_occupied_by_color(end, king.color)) {
                moves.push_back(Move(king.pos, end));
            }
        }
    }
    return moves;
}

std::vector<Move> MoveGenerator::get_castling_moves() const {
    std::vector<Move> castling_moves = get_potential_castling_moves();
    for (auto it = castling_moves.begin(); it != castling_moves.end();) {
        if (!is_valid_castling(*it)) {
            it = castling_moves.erase(it);
        } else {
            it->is_castling_move = true;
            ++it;
        }
    }
    return castling_moves;
}

std::vector<Move> MoveGenerator::get_potential_castling_moves() const {
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

bool MoveGenerator::is_valid_castling(const Move& move) const {
    int rook_x;
    if (move.end.x == 6) {
        rook_x = 7;
    } else if (move.end.x == 2) {
        rook_x = 0;
    } 
    
    const Pos king_pos = Pos{move.start.x, move.start.y};
    const Pos rook_pos = Pos{rook_x, move.start.y};
    if (!board_helper.is_clear_line(king_pos, rook_pos)) {
        return false;
    }
    if (is_in_check(board.game_state.player_to_move)) {
        return false;
    }
    if (passes_through_check_when_castling(move)) {
        return false;
    }

    return true;
}

bool MoveGenerator::passes_through_check_when_castling(const Move& castling_move) const {
    const int row = castling_move.start.y;
    const int start_x = castling_move.start.x;
    const int end_x = castling_move.end.x;
    const int direction = end_x - start_x > 0 ? 1 : -1;
    const Color opponent = get_opposite_color(board.game_state.player_to_move);

    std::vector<Move> threatened_moves = get_threatened_moves(opponent);
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


std::vector<Move> MoveGenerator::get_pawn_captures(Piece pawn) const {
    const int direction = pawn.color == BLACK ? 1 : -1;

    const Pos start = pawn.pos;
    const Pos end1 = Pos{pawn.pos.x + 1, pawn.pos.y + direction};
    const Pos end2 = Pos{pawn.pos.x - 1, pawn.pos.y + direction};

    std::vector<Move> moves;
    if (!is_outside_board(end1)) {
        moves.push_back(Move(start, end1));
    }
    if (!is_outside_board(end2)) {
        moves.push_back(Move(start, end2));
    }
    return moves;
}
