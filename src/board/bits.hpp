#pragma once

#include <stdint.h>
#include <string>

namespace bits {
int pop_lsb(uint64_t &bits);
std::string to_str(uint64_t bits);
uint64_t reverse(uint64_t bits);
} // namespace bits
