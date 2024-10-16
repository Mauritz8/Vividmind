#pragma once

#include <sys/types.h>

namespace bits {
u_int64_t get(u_int64_t bits, u_int64_t n);
void set(u_int64_t &bits, u_int64_t n);
void unset(u_int64_t &bits, u_int64_t n);
} // namespace bits
