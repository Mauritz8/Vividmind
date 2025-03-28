#pragma once

enum CommandType { GoInfinite, GoDepth, GoMoveTime, GoGameTime, GoPerft };
union CommandArg {
  int integer;
};

class Command {
public:
  CommandType type;
  union CommandArg arg;

  Command();
  static Command go_infinite();
  static Command go_depth(int depth);
  static Command go_move_time(int move_time);
  static Command go_game_time(int white_time, int black_time, int white_inc,
                            int black_inc, int moves_to_go);
  static Command go_perft(int depth);

private:

  Command(CommandType type);
  Command(CommandType type, int arg);

};
