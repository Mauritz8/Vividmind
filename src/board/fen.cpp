#include "board.hpp"

#include <sstream>
#include <string>


Board Board::get_starting_position() {
    return get_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board Board::get_position_from_fen(std::string fen) {
    std::istringstream fen_stream(fen);
    std::array<std::string, 6> fen_parts;

    int i = 0;
    std::string fen_part;
    while (std::getline(fen_stream, fen_part, ' ')) {
        fen_parts[i++] = fen_part;
    }

    if (i != 6) {
        throw std::invalid_argument("Invalid FEN: Does not contain six parts\n");
    }

    Board board;
    board.game_state.material[WHITE] = 0;
    board.game_state.material[BLACK] = 0;
    board.game_state.psqt[WHITE] = 0;
    board.game_state.psqt[BLACK] = 0;
    board.place_pieces(fen_parts[0]);
    board.set_player_to_move(fen_parts[1]);
    board.set_castling_rights(fen_parts[2]);
    board.set_en_passant_square(fen_parts[3]);
    board.game_state.halfmove_clock = std::stoi(fen_parts[4]);
    board.game_state.fullmove_number =  std::stoi(fen_parts[5]);
    return board;
}

void Board::place_pieces(const std::string& pieces) {
    int pos = 0;
    for (const char ch : pieces) {
        if (ch == '/') continue;

        if (isdigit(ch)) {
            const int n = (int) ch - '0';
            for (int i = 0; i < n; i++) {
                squares[pos].pos = pos;
                pos++;
            }
        } else {
            Color color = islower(ch) ? BLACK : WHITE;
            Piece piece = Piece(get_piece_type(ch), color, pos);
            Square& square = squares[pos];
            squares[pos].pos = pos;
            squares[pos].piece = piece;
            this->pieces[color].push_back(piece);
            game_state.material[color] += piece.get_value();                
            game_state.psqt[color] += get_psqt_score(piece);
            pos++;
        }
    }
}

void Board::set_player_to_move(const std::string& player_to_move) {
    if (player_to_move == "w") {
        this->game_state.player_to_move = WHITE;
    } else if (player_to_move == "b") {
        this->game_state.player_to_move = BLACK;
    } 
}

void Board::set_castling_rights(const std::string& castling_rights) {
    this->game_state.castling_rights[WHITE].kingside = false;
    this->game_state.castling_rights[WHITE].queenside = false;
    this->game_state.castling_rights[BLACK].kingside = false;
    this->game_state.castling_rights[BLACK].queenside = false;

    for (char ch : castling_rights) {
        switch (ch) {
            case 'K':
                this->game_state.castling_rights[WHITE].kingside = true;
                break;
            case 'Q':
                this->game_state.castling_rights[WHITE].queenside = true;
                break;
            case 'k':
                this->game_state.castling_rights[BLACK].kingside = true;
                break;
            case 'q':
                this->game_state.castling_rights[BLACK].queenside = true;
                break;
        }
    }
}

void Board::set_en_passant_square(const std::string& en_passant_square) {
    if (en_passant_square.size() != 2) {
        return;
    } 
    const int x = en_passant_square[0] - 'a';
    const int y = 8 - (en_passant_square[1] - '0');
    this->game_state.en_passant_square = x + y * 8;
}

