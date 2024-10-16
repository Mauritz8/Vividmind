#include "perft.hpp"
#include "fmt/core.h"

int perft(MailboxBoard &board, int depth) {
  if (depth == 0) {
    return 1;
  }

  int nodes = 0;
  const Color player = board.get_player_to_move();
  std::vector<Move> pseudo_legal_moves = board.get_pseudo_legal_moves(ALL);
  for (const Move &move : pseudo_legal_moves) {
    board.make(move);
    if (board.is_in_check(player)) {
      board.undo();
      continue;
    }
    nodes += perft(board, depth - 1);
    board.undo();
  }
  return nodes;
}

void divide(MailboxBoard &board, int depth) {
  int nodes_searched = 0;
  const Color player = board.get_player_to_move();
  std::vector<Move> pseudo_legal_moves = board.get_pseudo_legal_moves(ALL);
  for (const Move &move : pseudo_legal_moves) {
    board.make(move);
    if (board.is_in_check(player)) {
      board.undo();
      continue;
    }
    const int nodes = perft(board, depth - 1);
    nodes_searched += nodes;
    fmt::println("{}: {}", move.to_uci_notation(), nodes);
    board.undo();
  }
  fmt::println("\nNodes searched: {}", nodes_searched);
}
