#include "bits.hpp"
#include <byteswap.h>

namespace bits {

u_int64_t get(u_int64_t bits, int n) {
  return (bits >> n) & (u_int64_t)1;
}

void set(u_int64_t &bits, int n) { bits |= (u_int64_t)1 << n; }

void unset(u_int64_t &bits, int n) { bits ^= (u_int64_t)1 << n; }

std::optional<int> popLSB(u_int64_t &bits) {
  for (int i = 0; i < 64; i++) {
    u_int64_t bit = get(bits, i);
    if (bit == 1) {
      unset(bits, i);
      return i;
    }
  }
  return std::nullopt;
}

std::string to_string(u_int64_t bits) {
  std::string out;
  for (int row = 7; row >= 0; row--) {
    for (int col = 7; col >= 0; col--) {
      u_int64_t bit = get(bits, row * 8 + col);
      out += bit == 1 ? "1" : "0";
    }
    out += " ";
  }
  return out;
}

// TODO: Using a library function should be faster
// It would depend on the operating system,
// bswap on linux and __byteswap_uint64 on windows
// https://man7.org/linux/man-pages/man3/bswap.3.html
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/byteswap-uint64-byteswap-ulong-byteswap-ushort
// I tried it but it seems like it's not doing the same thing, I need to investigate
u_int64_t reverse(u_int64_t b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
         (b >> 48);
}

int nr_bits_set(u_int64_t bits) {
  int res = 0;
  std::optional<int> bit = popLSB(bits);
  while (bit.has_value()) {
    res++;
    bit = popLSB(bits);
  }
  return res;
}

} // namespace bits
