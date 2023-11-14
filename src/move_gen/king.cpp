#include "move_gen.hpp"
#include "utils.hpp"


std::vector<Move> MoveGenerator::get_king_normal_moves(const Piece& piece, MoveCategory move_category) const {
    std::array<int, 8> movements = {11, -11, 10, -10, 9, -9, 1, -1};
    const Color opponent = get_opposite_color(piece.color);
    std::vector<Move> moves;
    moves.reserve(8);
    for (int movement : movements) {
        const int end = mailbox[mailbox64[piece.pos] + movement];
        if (end == -1) {
            continue;
        }

        if (move_category == TACTICAL && board.squares[end].is_occupied_by(opponent)) {
            moves.push_back(Move(piece.pos, end));
        } else if (move_category == ALL && !board.squares[end].is_occupied_by(piece.color)) {
            moves.push_back(Move(piece.pos, end));
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
            it->move_type = CASTLING;
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
    if (!is_clear_path_castling(move)) {
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

bool MoveGenerator::is_clear_path_castling(const Move& castling_move) const {
    const int x_direction = castling_move.end - castling_move.start > 0 ? 1 : -1;
    const int rook_pos = x_direction == 1 
                         ? castling_move.end + 1
                         : castling_move.end - 2;

    int pos = castling_move.start + x_direction;
    while (pos != rook_pos) {
        if (board.squares[pos].piece) {
            return false;
        }
        pos += x_direction;
    }
    return true;
}

bool MoveGenerator::passes_through_check_when_castling(const Move& castling_move) const {
    const int direction = castling_move.end - castling_move.start > 0 ? 1 : -1;
    const Color player = board.game_state.player_to_move;
    const Color opponent = get_opposite_color(player);

    int pos = castling_move.start + direction;
    while (true) {
        if (is_attacked_by(pos, opponent)) {
            return true;
        }
        if (pos == castling_move.end) break; 
        pos += direction;
    }
    return false;
}
