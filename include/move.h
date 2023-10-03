#ifndef MOVE_H
#define MOVE_H

#include <optional>
#include <string>
#include <vector>

#include "piece.h"
#include "square.h"
#include "board.h"

class Move {
    public:
        Move() {}
        Move(const Square& start_square, const Square& end_square);
        Move(const std::string& uci_notation, Board& board);

        const Square& get_start_square() const;
        void set_start_square(const Square& start_square);
        const Square& get_end_square() const;
        void set_end_square(const Square& end_square);
        std::unique_ptr<Piece> get_captured_piece();
        void set_captured_piece(std::unique_ptr<Piece> captured_piece);
        bool is_castling_move() const;
        void set_castling_move(bool castling_move);
        bool is_promotion() const;
        void set_promotion(bool promotion);
        std::optional<Piece_type> get_promotion_piece() const;
        void set_promotion_piece(const std::optional<Piece_type> promotion_piece);
        bool is_en_passant() const;
        void set_en_passant(bool en_passant);

        bool operator==(const Move& move) const;

        bool is_threatened_move(const Board& board) const;
        bool leaves_king_in_check(const Board& board, const std::vector<Move>& move_history) const;
        bool is_legal(const Board& board, const std::vector<Move>& move_history) const;
        void make_appropriate(Board& board, std::vector<Move>& move_history);
        void undo_appropriate(Board& board, std::vector<Move>& move_history);
        std::string to_uci_notation() const;

    private:
        Square start_square;
        Square end_square;
        std::unique_ptr<Piece> captured_piece;
        bool castling_move;
        bool promotion;
        std::optional<Piece_type> promotion_piece;
        bool en_passant;

        void make(Board& board);
        void undo(Board& board);
        bool is_valid_rook_move(const Board& board) const;
        bool is_valid_bishop_move(const Board& board) const ;
        bool is_valid_queen_move(const Board& board) const;
        bool is_valid_knight_move() const;
        bool is_valid_king_move() const;
        bool is_valid_pawn_move(const Board& board) const;
        bool is_valid_pawn_move_threat(const Board& board) const;
        bool validate_basic(const Board& board) const;
        bool passes_through_check_when_castling(const Board& board, const std::vector<Move>& move_history) const;
        bool is_valid_castling(const Board& board, const std::vector<Move>& move_history) const;
        Move get_castling_rook_move(const Board& board) const;
        void make_castling(Board& board);
        void undo_castling(Board& board);
        void make_en_passant(Board& board);
        void undo_en_passant(Board& board);
        void make_promotion(Board& board);
        void undo_promotion(Board& board);
};

#endif
