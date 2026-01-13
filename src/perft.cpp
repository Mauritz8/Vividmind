#include "perft.hpp"
#include "board/board.hpp"
#include "fmt/core.h"

int perft(Board &board, int depth) {
  if (depth == 0) {
    return 1;
  }

  int nodes = 0;
  std::vector<Move> moves = board.get_legal_moves();
  for (const Move &move : moves) {
    board.make(move);
    nodes += perft(board, depth - 1);
    board.undo();
  }
  return nodes;
}

void divide(Board &board, int depth) {
  int nodes_searched = 0;
  std::vector<Move> moves = board.get_legal_moves();
  for (const Move &move : moves) {
    board.make(move);
    const int nodes = perft(board, depth - 1);
    nodes_searched += nodes;
    fmt::println("{}: {}", move.to_uci_notation(), nodes);
    board.undo();
  }
  fmt::println("\nNodes searched: {}", nodes_searched);
}
