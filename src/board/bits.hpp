#pragma once

#include <stdint.h>
#include <string>

namespace bits {
int popLSB(uint64_t &bits);
std::string to_str(uint64_t bits);
uint64_t reverse(uint64_t bits);
} // namespace bits
