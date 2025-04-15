#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

#ifdef _WIN32
#include <fileapi.h>
#include <namedpipeapi.h>
#else
#include <unistd.h>
#endif

#include "board.hpp"
#include "engine/command.hpp"
#include "engine/engine.hpp"
#include "uci.hpp"

#ifdef _WIN32
void read_input(HANDLE wd, std::atomic<bool> &stop) {
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    if (input == "stop") {
      stop = true;
    } else {
      Command command = uci::process(input);
      WriteFile(wd, &command, sizeof(command), NULL, NULL);
    }
  }
}
#else
void read_input(int wd, std::atomic<bool> &stop) {
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    if (input == "stop") {
      stop = true;
    } else {
      Command command = uci::process(input);
      write(wd, &command, sizeof(command));
    }
  }
}
#endif

#ifdef _WIN32
void run_engine(HANDLE rd, std::atomic<bool> &stop) {
  std::unique_ptr<Board> board = Board::get_starting_position();
  Command command;
  while (true) {
    ReadFile(rd, &command, sizeof(command), NULL, NULL);
    engine::execute_command(command, stop, board);
  }
}
#else
void run_engine(int rd, std::atomic<bool> &stop) {
  std::unique_ptr<Board> board = Board::get_starting_position();
  Command command;
  while (true) {
    read(rd, &command, sizeof(command));
    engine::execute_command(command, stop, board);
  }
}
#endif

int main() {
  std::atomic<bool> stop = false;
#ifdef _WIN32
  HANDLE rd;
  HANDLE wd;
  if (CreatePipe(&rd, &wd, NULL, 0) == 0) {
    exit(1);
  }
  std::thread t1(read_input, wd, std::ref(stop));
  std::thread t2(run_engine, rd, std::ref(stop));
#else
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    exit(1);
  }
  std::thread t1(read_input, pipefd[1], std::ref(stop));
  std::thread t2(run_engine, pipefd[0], std::ref(stop));
#endif
  t1.join();
  t2.join();

  return 0;
}
