#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <vector>

#include "board.hpp"
#include "board_helper.hpp"
#include "move.hpp"
#include "move_validator.hpp"
#include "piece.hpp"


class MoveGenerator {
    public:
        MoveGenerator(Board& board, BoardHelper& board_helper);

        std::vector<Move> get_legal_moves(bool only_captures) const;
        std::vector<Move> get_threatened_moves(Color color) const;
        int perft(int depth) const;
        void divide(int depth) const;
        bool leaves_king_in_check(const Move& move) const;
        bool is_in_check(Color color) const;

    private:
        Board& board;
        BoardHelper& board_helper;
        MoveValidator move_validator;

        std::vector<Move> get_psuedo_legal_moves(bool only_captures) const;

        std::vector<Move> get_psuedo_legal_moves(const Piece& piece, bool only_captures) const;
        std::vector<Move> get_threatened_moves(const Piece& piece) const;

        std::vector<Move> get_bishop_psuedo_legal_moves(const Piece& piece, bool only_captures) const;
        std::vector<Move> get_rook_psuedo_legal_moves(const Piece& piece, bool only_captures) const;
        std::vector<Move> get_queen_psuedo_legal_moves(const Piece& piece, bool only_captures) const;
        std::vector<Move> get_knight_psuedo_legal_moves(const Piece& piece, bool only_captures) const;
        std::vector<Move> get_king_psuedo_legal_moves(const Piece& piece, bool only_captures) const;
        std::vector<Move> get_pawn_psuedo_legal_moves(Piece pawn, bool only_captures) const;

        std::vector<Move> get_psuedo_legal_moves_direction(const Piece& piece, int x_direction, int y_direction, bool only_captures) const;

        std::vector<Move> get_king_threatened_moves(Piece king, bool only_captures) const;
        std::vector<Move> get_castling_moves() const;
        std::vector<Move> get_potential_castling_moves() const;
        bool is_valid_castling(const Move& move) const;
        bool passes_through_check_when_castling(const Move& castling_move) const;

        std::vector<Move> get_pawn_captures(Piece pawn) const;
};

#endif
