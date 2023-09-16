#include <iostream>
#include <string>
#include <vector>

#include "board.h"
#include "game_state.h"
#include "move.h"
#include "game_over_conditions.h"
#include "piece.h"
#include "engine/engine.h"

int main() {
    Board board = Board::get_starting_position();
    std::vector<Move> move_history;

    while (true) {
        board.show_board();

        if (is_checkmate(board, move_history)) {
            const Color winner = board.get_player_to_move() == WHITE ? BLACK : WHITE;
            std::cout << (winner == WHITE ? "White" : "Black") << " won!\n";
            break;
        } else if (is_draw(board, move_history)) {
            std::cout << "It's a draw\n";
            break;
        }

        Move move;
        bool legal_move = false;
        while (!legal_move) {
            std::string move_uci;
            std::cout << "Move: ";
            std::cin >> move_uci;
            move = Move(move_uci, board);

            if (move.is_legal_move(board, move_history)) {
                legal_move = true;
            } else {
                std::cout << "\nThat's not a legal move\n\n"; 
            }
        }

        move.make_appropriate_move(board, move_history);
    }
}
