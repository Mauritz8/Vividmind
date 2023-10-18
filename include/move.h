#ifndef MOVE_H
#define MOVE_H

#include <optional>
#include <string>
#include <vector>

#include "piece.h"
#include "square.h"
#include "board.h"

typedef struct {
    int x;
    int y;
} Pos;

class Move {
    public:
        Pos start;
        Pos end;
        std::shared_ptr<Piece> captured_piece;
        bool is_castling_move;
        bool is_promotion;
        std::optional<Piece_type> promotion_piece;
        bool is_en_passant;

        Move(int start_x, int start_y, int end_x, int end_y);
        Move(const Square& start, const Square& end);
        static Move get_from_uci_notation(const std::string& uci_notation, const Board& board, const std::vector<Move>& move_history);
        Move(const Move& move);

        Move operator=(const Move& move);
        bool operator==(const Move& move) const;

        bool leaves_king_in_check(const Board& board, const std::vector<Move>& move_history) const;
        void make_appropriate(Board& board, std::vector<Move>& move_history);
        void undo_appropriate(Board& board, std::vector<Move>& move_history);
        std::string to_uci_notation() const;

    private:

        void make(Board& board);
        void undo(Board& board);
        bool is_valid_rook_move(const Board& board) const;
        bool is_valid_bishop_move(const Board& board) const ;
        bool is_valid_queen_move(const Board& board) const;
        bool is_valid_knight_move() const;
        bool is_valid_king_move() const;
        bool is_valid_pawn_move(const Board& board) const;
        bool is_valid_pawn_move_threat(const Board& board) const;
        Move get_castling_rook_move(const Board& board) const;
        void make_castling(Board& board);
        void undo_castling(Board& board);
        void make_en_passant(Board& board);
        void undo_en_passant(Board& board);
        void make_promotion(Board& board);
        void undo_promotion(Board& board);
};

#endif
