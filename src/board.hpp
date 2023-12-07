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
        static Board get_position_from_fen(const std::string& fen);

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
        Color calc_player_to_move(const std::string& player_to_move);
        std::array<Castling, 2> calc_castling_rights(const std::string& castling_rights_str);
        std::optional<int> calc_en_passant_square(const std::string& en_passant_square);
        int calc_halfmove_clock(const std::string& halfmove_clock);
        int calc_fullmove_number(const std::string& fullmove_number);

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
