#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include <vector>

#include "board.hpp"
#include "defs.hpp"
#include "move.hpp"
#include "piece.hpp"


enum MoveCategory {ALL, TACTICAL};

class MoveGenerator {
    public:
        MoveGenerator(Board& board);

        std::vector<Move> get_pseudo_legal_moves(MoveCategory move_category) const;
        int perft(int depth) const;
        void divide(int depth) const;
        bool is_in_check(Color color) const;

    private:
        Board& board;

        std::vector<Move> get_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;
        bool is_attacked_by(int pos, Color color) const;

        std::vector<Move> get_bishop_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;
        std::vector<Move> get_rook_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;
        std::vector<Move> get_queen_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;
        std::vector<Move> get_knight_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;
        std::vector<Move> get_king_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;
        std::vector<Move> get_pawn_pseudo_legal_moves(const Piece& piece, MoveCategory move_category) const;

        std::vector<Move> get_pseudo_legal_moves_direction(const Piece& piece, int x_direction, int y_direction, MoveCategory move_category) const;

        std::vector<Move> get_king_normal_moves(const Piece& piece, MoveCategory move_category) const;
        std::vector<Move> get_castling_moves() const;
        std::vector<Move> get_potential_castling_moves() const;
        bool is_valid_castling(const Move& move) const;
        bool is_clear_path_castling(const Move& castling_move) const;
        bool passes_through_check_when_castling(const Move& castling_move) const;

        std::vector<Move> get_pawn_captures(const Piece& piece) const;
        bool is_valid_en_passant(int capture_pos) const;
};

#endif
