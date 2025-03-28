#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "search/engine.hpp"
#include "uci.hpp"

void read_input(int wd) {
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    uci::process(input, wd);
  }
}

int main() {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    exit(1);
  }

  std::thread t1(read_input, pipefd[1]);
  std::thread t2(engine::run, pipefd[0]);

  t1.join();
  t2.join();

  return 0;
}
