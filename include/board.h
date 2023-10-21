#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>


struct Castling {
    bool kingside;
    bool queenside;
};

struct GameState {
    Color player_to_move;
    std::optional<Pos> en_passant_square;
    std::array<Castling, 2> castling_rights;
    std::array<std::vector<std::shared_ptr<Piece>>, 2> pieces;
    std::array<int, 2> material;
    std::array<int, 2> psqt;
};

class Board {
    public:
        GameState game_state;
        std::vector<GameState> history;

        Board() {}

        static Board get_starting_position();
        static Board get_position_from_fen(std::string fen);
        
        const Square& get_square(int x, int y) const;
        Square& get_square(int x, int y);
        const Square& get_square(Pos pos) const;
        Square& get_square(Pos pos);

        void show() const;
        void switch_player_to_move();
        std::vector<Move> get_psuedo_legal_moves();
        std::vector<Move> get_threatened_moves(Color color);
        void remove_piece(std::shared_ptr<Piece> piece);

    private:
        std::array<std::array<Square, 8>, 8> squares;

        void place_pieces(const std::string& pieces);
        void set_player_to_move(const std::string& player_to_move);
        void set_castling_rights(const std::string& castling_rights);
        void set_en_passant_square(const std::string& en_passant_square);
};

#endif
