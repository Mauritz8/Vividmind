#pragma once

#include <optional>
#include <string>
#include <sys/types.h>

namespace bits {
u_int64_t get(u_int64_t bits, int n);
void set(u_int64_t &bits, int n);
void unset(u_int64_t &bits, int n);
std::optional<int> popLSB(u_int64_t &bits);
std::string to_string(u_int64_t bits);
u_int64_t reverse(u_int64_t bits);
int nr_bits_set(u_int64_t bits);
} // namespace bits
