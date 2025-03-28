#include <atomic>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "engine/engine.hpp"
#include "uci.hpp"

void read_input(int wd, std::atomic<bool> &stop) {
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    uci::process(input, wd, stop);
  }
}

int main() {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    exit(1);
  }

  std::atomic<bool> stop = false;

  std::thread t1(read_input, pipefd[1], std::ref(stop));
  std::thread t2(engine::run, pipefd[0], std::ref(stop));

  t1.join();
  t2.join();

  return 0;
}
