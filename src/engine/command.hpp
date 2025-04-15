#pragma once

#include <string>
#include <vector>

enum CommandType {
  UCI,
  IsReady,
  Quit,
  Invalid,
  GoInfinite,
  GoDepth,
  GoMoveTime,
  GoGameTime,
  GoPerft,
  UpdateBoard,
};

struct GameTime {
  int wtime;
  int btime;
  int winc;
  int binc;
  int moves_to_go;
};

struct Position {
  char *fen;
  char **moves;
  size_t moves_size;
};

union CommandArg {
  int integer;
  char *str;
  GameTime game_time;
  Position position;
};

class Command {
public:
  CommandType type;
  union CommandArg arg;

  Command();

  static Command uci();
  static Command is_ready();
  static Command quit();
  static Command invalid(std::string_view str);
  static Command go_infinite();
  static Command go_depth(int depth);
  static Command go_move_time(int move_time);
  static Command go_game_time(int white_time, int black_time, int white_inc,
                              int black_inc, int moves_to_go);
  static Command go_perft(int depth);
  static Command update_board(const std::string &fen,
                              const std::vector<std::string> moves);

private:
  Command(CommandType type);
  Command(CommandType type, int arg);
  Command(CommandType type, char *arg);
  Command(CommandType type, GameTime game_time);
  Command(CommandType type, Position position);
};
