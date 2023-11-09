#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "defs.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "board/defs.hpp"


class Board {
    public:
        std::array<Square, 64> squares;
        std::array<std::vector<Piece>, 2> pieces;
        GameState game_state;
        std::vector<GameState> history;

        Board() {}

        static Board get_starting_position();
        static Board get_position_from_fen(std::string fen);

        bool operator==(const Board& board) const;

        void show() const;
        void switch_player_to_move();
        int get_king_square(Color color) const;

        void make(const Move& move);
        void undo();

        bool is_insufficient_material() const;
        bool is_threefold_repetition() const;
        bool is_draw_by_fifty_move_rule() const;

        int evaluate() const;

    private:
        void place_pieces(const std::string& pieces);
        void set_player_to_move(const std::string& player_to_move);
        void set_castling_rights(const std::string& castling_rights);
        void set_en_passant_square(const std::string& en_passant_square);

        Piece& get_piece(Piece piece);
        void remove_piece(Piece piece);
        void move_piece(Square& from, Square& to);
        void update_castling_rights(const Move& move);
        Move get_castling_rook_move(const Move& move) const;

        int get_psqt_score(const Piece& piece) const;
        bool is_lone_king(Color color) const;
        bool is_endgame() const;
};

#endif
