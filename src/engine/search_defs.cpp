#include "search_defs.hpp"

SearchParams::SearchParams() {
  depth = MAX_PLY;
  allocated_time = 0;
  search_mode = INFINITE;
}

SearchInfo::SearchInfo() {
  start_time = std::chrono::high_resolution_clock::now();
  depth = 0;
  ply_from_root = 0;
  seldepth = 0;
  nodes = 0;
  is_terminated = false;
}

int SearchInfo::time_elapsed() const {
  auto now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
      .count();
}
