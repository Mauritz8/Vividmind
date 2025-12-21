#include "board/board.hpp"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "engine/command.hpp"
#include "engine/engine.hpp"
#include "uci.hpp"

void read_input(std::queue<Command> &commands, std::condition_variable &cv,
                std::mutex &mtx, std::atomic<bool> &stop) {
  std::string input;
  while (std::getline(std::cin, input)) {
    if (input == "stop") {
      stop = true;
      continue;
    }

    Command cmd = uci::process(input);
    {
      std::lock_guard<std::mutex> lock(mtx);
      commands.push(cmd);
    }
    cv.notify_one();

    if (cmd.type == CommandType::Quit) {
      return;
    }
  }

  {
    std::lock_guard<std::mutex> lock(mtx);
    commands.push(Command::quit());
  }
  cv.notify_one();
}

void run_engine(std::queue<Command> &commands, std::condition_variable &cv,
                std::mutex &mtx, std::atomic<bool> &stop) {
  Board board = Board::get_starting_position();
  while (true) {
    Command cmd;
    {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [&] { return !commands.empty(); });
      cmd = commands.front();
      commands.pop();
    }

    if (cmd.type == CommandType::Quit) {
      return;
    }
    engine::execute_command(cmd, stop, board);
  }
}

int main() {
  std::queue<Command> commands;
  std::condition_variable cv;
  std::mutex mtx;
  std::atomic<bool> stop = false;

  std::thread t1(read_input, std::ref(commands), std::ref(cv), std::ref(mtx),
                 std::ref(stop));
  std::thread t2(run_engine, std::ref(commands), std::ref(cv), std::ref(mtx),
                 std::ref(stop));

  t1.join();
  t2.join();
  return 0;
}
