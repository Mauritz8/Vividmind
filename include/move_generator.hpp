#ifndef MOVE_GENERATOR_HPP
#define MOVE_GENERATOR_HPP

#include <vector>

#include "board.hpp"
#include "move.hpp"
#include "move_validator.hpp"
#include "piece.hpp"


enum MoveType {ALL, CAPTURES};

class MoveGenerator {
    public:
        MoveGenerator(Board& board);

        std::vector<Move> get_pseudo_legal_moves(MoveType move_type) const;
        std::vector<Move> get_threatened_moves(Color color) const;
        int perft(int depth) const;
        void divide(int depth) const;
        bool is_in_check(Color color) const;

    private:
        Board& board;
        MoveValidator move_validator;


        std::vector<Move> get_pseudo_legal_moves(const Piece& piece, MoveType move_type) const;
        std::vector<Move> get_threatened_moves(const Piece& piece) const;

        std::vector<Move> get_bishop_pseudo_legal_moves(const Piece& piece, MoveType move_type) const;
        std::vector<Move> get_rook_pseudo_legal_moves(const Piece& piece, MoveType move_type) const;
        std::vector<Move> get_queen_pseudo_legal_moves(const Piece& piece, MoveType move_type) const;
        std::vector<Move> get_knight_pseudo_legal_moves(const Piece& piece, MoveType move_type) const;
        std::vector<Move> get_king_pseudo_legal_moves(const Piece& piece, MoveType move_type) const;
        std::vector<Move> get_pawn_pseudo_legal_moves(Piece pawn, MoveType move_type) const;

        std::vector<Move> get_pseudo_legal_moves_direction(const Piece& piece, int x_direction, int y_direction, MoveType move_type) const;

        std::vector<Move> get_king_threatened_moves(Piece king, MoveType move_type) const;
        std::vector<Move> get_castling_moves() const;
        std::vector<Move> get_potential_castling_moves() const;
        bool is_valid_castling(const Move& move) const;
        bool passes_through_check_when_castling(const Move& castling_move) const;

        std::vector<Move> get_pawn_captures(Piece pawn) const;
};

#endif
