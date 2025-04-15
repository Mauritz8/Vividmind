#include "command.hpp"
#include <vector>

Command::Command() { this->type = CommandType::GoInfinite; }

Command::Command(CommandType type) { this->type = type; }

Command::Command(CommandType type, int arg) {
  this->type = type;
  this->arg.integer = arg;
}

Command::Command(CommandType type, char *arg) {
  this->type = type;
  this->arg.str = arg;
}

Command::Command(CommandType type, GameTime game_time) {
  this->type = type;
  this->arg.game_time = game_time;
}

Command::Command(CommandType type, Position position) {
  this->type = type;
  this->arg.position = position;
}
Command Command::uci() {
  return Command(CommandType::UCI);
}

Command Command::is_ready() {
  return Command(CommandType::IsReady);
}

Command Command::quit() {
  return Command(CommandType::Quit);
}

static char *str_to_c_str(const std::string_view str) {
  size_t str_size = str.size();
  char *c_str = (char *)calloc(str_size + 1, sizeof(char));
  for (size_t i = 0; i < str_size; i++) {
    c_str[i] = str.at(i);
  }
  return c_str;
}

Command Command::invalid(std::string_view str) {
  char *c_str = str_to_c_str(str);
  return Command(CommandType::Invalid, c_str);
}

Command Command::go_infinite() { return Command(CommandType::GoInfinite); };

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

Command Command::update_board(const std::string &fen,
                              const std::vector<std::string> moves) {
  size_t moves_size = moves.size();
  char **moves_c_array = (char **)calloc(moves_size, sizeof(char *));
  for (size_t i = 0; i < moves_size; i++) {
    moves_c_array[i] = str_to_c_str(moves.at(i));
  }

  Position position = {
      .fen = str_to_c_str(fen),
      .moves = moves_c_array,
      .moves_size = moves_size,
  };
  return Command(CommandType::UpdateBoard, position);
}
