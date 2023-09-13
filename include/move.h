#ifndef MOVE_H
#define MOVE_H

#include <optional>
#include <string>
#include <vector>

#include "piece.h"
#include "square.h"
#include "board.h"

class Move {
    private:
        Square start_square;
        Square end_square;
        std::optional<Piece> captured_piece;
        bool castling_move;
        bool promotion;
        std::optional<Piece_type> promotion_piece;
        bool en_passant;

        void make_move(Board& board);
        void undo_move(Board& board);
        bool is_valid_rook_move(const Board& board) const;
        bool is_valid_bishop_move(const Board& board) const ;
        bool is_valid_queen_move(const Board& board) const;
        bool is_valid_knight_move() const;
        bool is_valid_king_move() const;
        bool is_valid_pawn_move(const Board& board) const;
        bool is_valid_pawn_move_threat(const Board& board) const;
        bool validate_move_basic(const Board& board) const;
        bool is_valid_piece_movement(const Board& board) const;
        bool passes_through_check_when_castling(const Board& board, const std::vector<Move> move_history) const;
        bool is_valid_castling_move(const Board& board, const std::vector<Move> move_history) const;
        Move get_castling_rook_move(const Board& board) const;
        void make_castling_move(Board& board);
        void undo_castling_move(Board& board);
        bool is_valid_en_passant_move(const Board& board, const std::vector<Move> move_history) const;
        void make_en_passant_move(Board& board);
        void undo_en_passant_move(Board& board);
        void make_promotion_move(Board& board);
        void undo_promotion_move(Board& board);

    public:
        Move() {}

        Move(Square start_square, Square end_square) {
            this->set_start_square(start_square);
            this->set_end_square(end_square);
        }

        Move(std::string uci_notation, Board& board) {
            const int start_x = uci_notation[0] - 'a';
            const int start_y = 8 - (uci_notation[1] - '0');
            const int end_x = uci_notation[2] - 'a';
            const int end_y = 8 - (uci_notation[3] - '0');
            const Square& start = board.get_square(start_x, start_y);
            const Square& end = board.get_square(end_x, end_y);
            this->set_start_square(start);
            this->set_end_square(end);

            if (uci_notation.length() == 5) {
                const char promotion_piece = uci_notation[4];
                this->set_promotion_piece(get_promotion_piece_type(promotion_piece));
            }
        }

        Square get_start_square() const {
            return start_square;
        }

        void set_start_square(Square start_square) {
            this->start_square = start_square;
        }

        Square get_end_square() const {
            return end_square;
        }

        void set_end_square(Square end_square) {
            this->end_square = end_square;
        }

        std::optional<Piece> get_captured_piece() const {
            return captured_piece;
        }

        void set_captured_piece(std::optional<Piece> captured_piece) {
            this->captured_piece = captured_piece; 
        }

        bool is_castling_move() const {
            return castling_move;
        }

        void set_castling_move(bool castling_move) {
            this->castling_move = castling_move;
        }

        bool is_promotion() const {
            return promotion;
        }

        void set_promotion(bool promotion) {
            this->promotion = promotion;
        }

        std::optional<Piece_type> get_promotion_piece() const {
            return promotion_piece;
        }

        void set_promotion_piece(std::optional<Piece_type> promotion_piece) {
            this->promotion_piece = promotion_piece;
        }

        bool is_en_passant() const {
            return en_passant;
        }

        void set_en_passant(bool en_passant) {
            this->en_passant = en_passant;
        }


        bool validate_threatened_move(const Board& board) const;
        bool leaves_king_in_check(const Board& board, const std::vector<Move> move_history) const;
        bool is_promotion_move(const Board& board) const;
        bool is_valid_promotion_move(const Board& board) const;
        bool is_legal_move(const Board& board, const std::vector<Move> move_history) const;
        void make_appropriate_move(Board& board, std::vector<Move> move_history);
        void undo_appropriate_move(Board& board, std::vector<Move> move_history);
        std::string move_to_uci_notation() const;
};

#endif
