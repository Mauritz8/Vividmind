#include "uci.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "defs.hpp"
#include "move.hpp"
#include "move_gen.hpp"
#include "search/defs.hpp"


UCI::UCI(Board& board) 
    : board(board)
    , move_gen(board)
    , search(board, move_gen)
{}

std::vector<std::string> _str_split(std::string_view str, char delim) {
    std::vector<std::string> substrings;    
    std::string substr = "";
    for (char ch : str) {
        if (ch == delim) {
            substrings.push_back(substr);
            substr = "";
        } else {
            substr += ch;
        }
    }
    return substrings;
}

void UCI::process(const std::string& input) {
    const std::vector<std::string> words = _str_split(input, ' ');

    const bool is_go_perft =
        words.size() == 3 && words.at(0) == "go" && words.at(1) == "perft";

    if (input == "uci") {
        uci();
    } else if (input == "isready") {
        isready();
    } else if (words.at(0) == "position") {
        position(input);
    } else if (words.at(0) == "go") {
        search.params = get_search_params(words);
        search.iterative_deepening_search();
    } else if (is_go_perft) {
        int depth = std::stoi(words.at(2));
        move_gen.divide(depth);
    } else if (input == "quit") {
        exit(0);
    }
    std::cout.flush();
}

void UCI::show(const SearchSummary& search_summary) {
    std::cout << "info";
    std::cout << " depth " << search_summary.depth;
    std::cout << " seldepth " << search_summary.seldepth;
    std::cout << " multipv 1";
    if (std::abs(search_summary.score) > CHECKMATE_THRESHOLD) {
        const int ply = CHECKMATE - std::abs(search_summary.score);
        const int mate_in_x = std::ceil(ply / 2.0);
        const int sign = search_summary.score > 0 ? 1 : -1;
        std::cout << " score mate " << sign * mate_in_x;
    } else {
        std::cout << " score cp " << search_summary.score;
    }
    std::cout << " nodes " << search_summary.nodes;
    const long long nps = search_summary.time == 0 
        ? search_summary.nodes * 1000
        : search_summary.nodes * 1000 / search_summary.time;
    std::cout << " nps " << nps;
    std::cout << " time " << search_summary.time;
    std::cout << " pv";
    for (const Move& move : search_summary.pv) {
        std::cout << " " << move.to_uci_notation();
    }
    std::cout << std::endl;
}

void UCI::bestmove(const Move& move) {
    std::cout << "bestmove " << move.to_uci_notation() << "\n";
}

void UCI::uci() {
    std::cout << "id name " << NAME << " " << VERSION << "\n";
    std::cout << "id author " << AUTHOR << "\n";
    std::cout << "uciok\n\n";
}

void UCI::isready() {
    std::cout << "readyok\n\n";
}

void UCI::position(std::istringstream& arguments) {
    std::string token;
    std::getline(arguments, token, ' ');
    if (token == "startpos") {
        board = Board::get_starting_position();
    } else if (token == "fen") {
        std::string fen;
        std::string fen_part;

        int i = 0;
        while (i < 6 && std::getline(arguments, fen_part, ' ')) {
            fen += fen_part;
            if (i < 5) {
                fen += ' ';
            }
            i++;
        }
        try {
            board = Board::get_position_from_fen(fen);
        } catch (const std::invalid_argument& e) {
            std::cout << e.what(); 
        }
    } else return;

    std::getline(arguments, token, ' ');
    if (token == "moves") {
        make_moves(arguments);
    }
}

SearchParams UCI::get_search_params(const std::vector<std::string>& words) const {
    SearchParams search_params = SearchParams();
    for (int i = 1; i < words.size() - 2; i += 2) {
        std::string name = words.at(i);
        std::string value = words.at(i + 1);
        if (name == "wtime") {
            search_params.game_time.wtime = std::stoi(value);
        } else if (name == "btime") {
            search_params.game_time.btime = std::stoi(value);
        } else if (name == "winc") {
            search_params.game_time.winc = std::stoi(value);
        } else if (name == "binc") {
            search_params.game_time.binc = std::stoi(value);
        } else if (name == "movestogo") {
            search_params.game_time.moves_to_go = std::stoi(value);
        } else if (name == "depth") {
            int depth = std::stoi(value);
            search_params.search_mode = DEPTH;
            search_params.depth = depth;
        } else if (name == "movetime") {
            int movetime =  std::stoi(value);
            search_params.search_mode = MOVE_TIME;
            search_params.allocated_time = movetime - MOVE_OVERHEAD;
        }
    
        if (search_params.game_time.wtime != 0 &&
            search_params.game_time.btime != 0) 
        {
            search_params.search_mode = MOVE_TIME;
            search_params.allocated_time = search.calc_allocated_time();
        }
    }
    return search_params;
}


void UCI::make_moves(std::istringstream& moves) {
    std::string move_uci;
    while (std::getline(moves, move_uci, ' ')) {
        const bool is_legal = make_move(move_uci);
        if (!is_legal) {
            return;
        }
    }
}

bool UCI::make_move(const std::string& move_uci) {
    MoveGenerator move_gen(board);
    const Color player = board.game_state.player_to_move;
    const std::vector<Move> pseudo_legal_moves = move_gen.get_pseudo_legal_moves(ALL);
    for (const Move& move : pseudo_legal_moves) {
        if (move.to_uci_notation() == move_uci) {
            board.make(move);
            if (move_gen.is_in_check(player)) {
                board.undo();
                return false;
            }
            return true;
        } 
    }
    return false;
}
