#include "move_generator.hpp"

#include <array>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

#include "utils.hpp"
#include "square.hpp"


MoveGenerator::MoveGenerator(Board& board)
    : board(board)
    , move_validator(board)
{}

std::vector<Move> MoveGenerator::get_pseudo_legal_moves(MoveType move_type) const {
    std::vector<Move> moves;
    for (Piece piece : board.game_state.pieces[board.game_state.player_to_move]) {
        std::vector<Move> piece_moves = get_pseudo_legal_moves(piece, move_type);
        moves.insert(std::end(moves), std::begin(piece_moves), std::end(piece_moves));
    } 
    return moves;
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
    const Color player = board.game_state.player_to_move;
    std::vector<Move> pseudo_legal_moves = get_pseudo_legal_moves(ALL);
    for (const Move& move : pseudo_legal_moves) {
        board.make(move);
        if (is_in_check(player)) {
            board.undo();
            continue;
        }
        nodes += perft(depth - 1);    
        board.undo();
    }
    return nodes;
}

void MoveGenerator::divide(int depth) const {
    std::cout << "";
    int nodes_searched = 0;
    const Color player = board.game_state.player_to_move;
    std::vector<Move> pseudo_legal_moves = get_pseudo_legal_moves(ALL);
    for (const Move& move : pseudo_legal_moves) {
        board.make(move);
        if (is_in_check(player)) {
            board.undo();
            continue;
        }
        const int nodes = perft(depth - 1);
        nodes_searched += nodes;
        std::cout << move.to_uci_notation() << ": " << nodes << "\n";
        board.undo();
    }
    std::cout << "\nNodes searched: " << nodes_searched << "\n";
}

bool MoveGenerator::is_in_check(Color color) const {
    const int king_square = board.get_king_square(color);
    const Color opponent = get_opposite_color(color);
    const std::vector<Move> pseudo_legal_moves = get_threatened_moves(opponent);
    for (const Move& move : pseudo_legal_moves) {
        if (move.end == king_square) {
            return true;
        }
    }
    return false;
}

std::vector<Move> MoveGenerator::get_pseudo_legal_moves(const Piece& piece, MoveType move_type) const {
    switch (piece.piece_type) {
        case KING: {
            return get_king_pseudo_legal_moves(piece, move_type);
        }
        case QUEEN: {
            return get_queen_pseudo_legal_moves(piece, move_type);
        }
        case ROOK: {
            return get_rook_pseudo_legal_moves(piece, move_type);
        }
        case BISHOP: {
            return get_bishop_pseudo_legal_moves(piece, move_type);
        }
        case KNIGHT: {
            return get_knight_pseudo_legal_moves(piece, move_type);
        }
        case PAWN: {
            return get_pawn_pseudo_legal_moves(piece, move_type);
        }
    }    
    return {};
}

std::vector<Move> MoveGenerator::get_threatened_moves(const Piece& piece) const {
    if (piece.piece_type == PAWN) {
        return get_pawn_captures(piece);
    }
    if (piece.piece_type == KING) {
        return get_king_threatened_moves(piece, ALL);
    }
    return get_pseudo_legal_moves(piece, ALL);
}


std::vector<Move> MoveGenerator::get_bishop_pseudo_legal_moves(const Piece& piece, MoveType move_type) const {
    std::vector<Move> moves;

    std::vector<Move> up_right = get_pseudo_legal_moves_direction(piece, 1, 1, move_type);
    std::vector<Move> down_right = get_pseudo_legal_moves_direction(piece, 1, -1, move_type);
    std::vector<Move> down_left = get_pseudo_legal_moves_direction(piece, -1, -1, move_type);
    std::vector<Move> up_left = get_pseudo_legal_moves_direction(piece, -1, 1, move_type);

    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));

    return moves;
}

std::vector<Move> MoveGenerator::get_rook_pseudo_legal_moves(const Piece& piece, MoveType move_type) const {
    std::vector<Move> moves;

    std::vector<Move> up = get_pseudo_legal_moves_direction(piece, 0, 1, move_type);
    std::vector<Move> right = get_pseudo_legal_moves_direction(piece, 1, 0, move_type);
    std::vector<Move> down = get_pseudo_legal_moves_direction(piece, 0, -1, move_type);
    std::vector<Move> left = get_pseudo_legal_moves_direction(piece, -1, 0, move_type);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(left), std::end(left));

    return moves;
}

std::vector<Move> MoveGenerator::get_queen_pseudo_legal_moves(const Piece& piece, MoveType move_type) const {
    std::vector<Move> moves;

    std::vector<Move> bishop_moves = get_bishop_pseudo_legal_moves(piece, move_type);
    std::vector<Move> rook_moves = get_rook_pseudo_legal_moves(piece, move_type);
    moves.insert(std::end(moves), std::begin(bishop_moves), std::end(bishop_moves));
    moves.insert(std::end(moves), std::begin(rook_moves), std::end(rook_moves));

    return moves;
}

std::vector<Move> MoveGenerator::get_knight_pseudo_legal_moves(const Piece& piece, MoveType move_type) const {
    const std::array<int, 8> movements = {21, -21, 19, -19, 12, -12, 8, -8};
    const Color opponent = get_opposite_color(piece.color);
    std::vector<Move> moves;
    moves.reserve(8);
    for (int movement : movements) {
        const int end = Board::mailbox[Board::mailbox64[piece.pos] + movement];
        if (end == -1) {
            continue;
        }

        if (move_type == CAPTURES && board.squares[end].is_occupied_by(opponent)) {
            moves.push_back(Move(piece.pos, end));
        } else if (move_type == ALL && !board.squares[end].is_occupied_by(piece.color)) {
            moves.push_back(Move(piece.pos, end));
        }
    }
    return moves;
}

std::vector<Move> MoveGenerator::get_king_pseudo_legal_moves(const Piece& piece, MoveType move_type) const {
    std::vector<Move> moves = get_king_threatened_moves(piece, move_type);
    moves.reserve(10);
    if (move_type == ALL) {
        std::vector<Move> castling_moves = get_castling_moves();
        moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
    }
    return moves;
}

std::vector<Move> MoveGenerator::get_pawn_pseudo_legal_moves(Piece pawn, MoveType move_type) const {
    std::vector<Move> moves;
    moves.reserve(4);
    const int direction = board.game_state.player_to_move == BLACK ? 1 : -1;
    int start = pawn.pos;

    if (move_type == ALL) {
        const int end1 = start + 8 * direction;
        if (!board.squares[end1].piece) {
            moves.push_back(Move(start, end1));

            const int starting_row = pawn.color == BLACK ? 1 : 6;
            if (start / 8 == starting_row) {
                const int end2 = start + 16 * direction;
                if (!board.squares[end2].piece) {
                    Move move = Move(start, end2);
                    move.is_pawn_two_squares_forward = true;
                    moves.push_back(move);
                }
            }
        }
    }


    std::vector<Move> captures = get_pawn_captures(pawn);
    for (Move& capture : captures) {
        const Square& end = board.squares[capture.end];
        if (end.piece && end.piece->color != pawn.color) {
            moves.push_back(capture);
        } else if (move_validator.is_valid_en_passant(capture)) {
            capture.is_en_passant = true;
            moves.push_back(capture);
        }
    }

    int about_to_promote_row = board.game_state.player_to_move == WHITE ? 1 : 6;
    if (start / 8 == about_to_promote_row) {
        const std::array<PieceType, 3> other_promotion_piece_types = {
            ROOK,
            BISHOP,
            KNIGHT
        };
        std::vector<Move> other_promotion_moves;
        for (Move& move : moves) {
            move.is_promotion = true;
            move.promotion_piece = QUEEN;
            for (PieceType piece_type : other_promotion_piece_types) {
                Move promotion_move = move;
                promotion_move.promotion_piece = piece_type;
                other_promotion_moves.push_back(promotion_move);
            }
        }
        moves.insert(moves.end(), other_promotion_moves.begin(), other_promotion_moves.end());
    }

    return moves;
}

std::vector<Move> MoveGenerator::get_pseudo_legal_moves_direction(const Piece& piece, int x_direction, int y_direction, MoveType move_type) const {
    std::vector<Move> moves;

    const int step = x_direction + y_direction * 10;
    int pos = Board::mailbox[Board::mailbox64[piece.pos] + step];
    while (pos != -1) {
        const Square& end = board.squares[pos];
        if (end.piece) {
            if (end.piece->color != piece.color) {
                moves.push_back(Move(piece.pos, pos));
            }
            break;
        }

        if (move_type == ALL) {
            moves.push_back(Move(piece.pos, pos));
        }
        pos = Board::mailbox[Board::mailbox64[pos] + step];
    }

    return moves;
}

std::vector<Move> MoveGenerator::get_king_threatened_moves(Piece king, MoveType move_type) const {
    std::array<int, 8> movements = {11, -11, 10, -10, 9, -9, 1, -1};
    const Color opponent = get_opposite_color(king.color);
    std::vector<Move> moves;
    moves.reserve(8);
    for (int movement : movements) {
        const int end = Board::mailbox[Board::mailbox64[king.pos] + movement];
        if (end == -1) {
            continue;
        }

        if (move_type == CAPTURES && board.squares[end].is_occupied_by(opponent)) {
            moves.push_back(Move(king.pos, end));
        } else if (move_type == ALL && !board.squares[end].is_occupied_by(king.color)) {
            moves.push_back(Move(king.pos, end));
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
    const int king_start = color == BLACK ? 4 : 60;

    std::vector<Move> potential_castling_moves;
    potential_castling_moves.reserve(2);
    if (board.game_state.castling_rights[color].kingside) {
        const int kingside_end = color == BLACK ? 6 : 62;
        potential_castling_moves.push_back(Move(king_start, kingside_end));
    } 

    if (board.game_state.castling_rights[color].queenside) {
        const int queenside_end = color == BLACK ? 2 : 58;
        potential_castling_moves.push_back(Move(king_start, queenside_end));
    } 

    return potential_castling_moves;
}

bool MoveGenerator::is_valid_castling(const Move& move) const {
    const int king_pos = move.start;
    int rook_pos;
    if (move.end % 8 == 6) {
        rook_pos = king_pos + 3;
    } else {
        rook_pos = king_pos - 4;
    }
    if (!move_validator.is_clear_line(king_pos, rook_pos)) {
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
    const int direction = castling_move.end - castling_move.start > 0 ? 1 : -1;
    const Color opponent = get_opposite_color(board.game_state.player_to_move);

    std::vector<Move> threatened_moves = get_threatened_moves(opponent);
    for (Move& move : threatened_moves) {
        if (move.end >= castling_move.start && move.end < castling_move.end) {
            return true;
        }
        if (move.end <= castling_move.start && move.end > castling_move.end) {
            return true;
        }
    }
    return false;
}


std::vector<Move> MoveGenerator::get_pawn_captures(Piece pawn) const {
    const int direction = pawn.color == BLACK ? 1 : -1;
    const std::array<int, 2> movements = {9 * direction, 11 * direction};

    std::vector<Move> moves;
    moves.reserve(2);
    for (int movement : movements) {
        int end = Board::mailbox[Board::mailbox64[pawn.pos] + movement];
        if (end != -1) {
           moves.push_back(Move(pawn.pos, end));  
        }
    }
    return moves;
}
