#include "bits.hpp"

namespace bits {
u_int64_t get(u_int64_t bits, u_int64_t n) {
  return (bits >> n) & (u_int64_t)1;
}
void set(u_int64_t &bits, u_int64_t n) { bits |= (u_int64_t)1 << n; }
void unset(u_int64_t &bits, u_int64_t n) { bits &= ~((u_int64_t)1 << n); }
} // namespace bits
