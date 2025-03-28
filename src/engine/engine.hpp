#pragma once

#include <atomic>
namespace engine {
void run(int read_descriptor, std::atomic<bool> &stop);
};
