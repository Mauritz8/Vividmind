#include "command.hpp"

Command::Command() {
  this->type = CommandType::GoInfinite;
}

Command::Command(CommandType type) {
  this->type = type;
}

Command::Command(CommandType type, int arg) {
  this->type = type;
  this->arg.integer = arg;
}

Command::Command(CommandType type, GameTime game_time) {
  this->type = type;
  this->arg.game_time = game_time;
}

Command::Command(CommandType type, const char *fen) {
  this->type = type;
  this->arg.str = fen;
}

Command Command::go_infinite() {
   return Command(CommandType::GoInfinite);
};

Command Command::go_depth(int depth) {
   return Command(CommandType::GoDepth, depth);
};

Command Command::go_move_time(int move_time) {
  return Command(CommandType::GoMoveTime, move_time);
}

Command Command::go_game_time(int white_time, int black_time, int white_inc,
                          int black_inc, int moves_to_go) {
  GameTime game_time = {
    .wtime = white_time,
    .btime = black_time,
    .winc = white_inc,
    .binc = black_inc,
    .moves_to_go = moves_to_go,
  };
  return Command(CommandType::GoGameTime, game_time);
}

Command Command::go_perft(int depth) {
  return Command(CommandType::GoPerft, depth);
}

Command Command::update_board(const std::string &fen) {
  size_t fen_size = fen.size();
  char *str = (char *) calloc(fen_size + 1, sizeof(char));
  for (size_t i = 0; i < fen_size; i++) {
    str[i] = fen.at(i);
  }
  return Command(CommandType::UpdateBoard, str);
}
