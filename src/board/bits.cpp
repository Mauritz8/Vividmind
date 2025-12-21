#include "bits.hpp"
#include <bit>
#include <strings.h>

namespace bits {

int popLSB(uint64_t &bits) {
  const int i = std::countr_zero(bits);
  bits ^= (uint64_t)1 << i;
  return i;
}

std::string to_str(uint64_t bits) {
  std::string out;
  for (int row = 7; row >= 0; row--) {
    for (int col = 7; col >= 0; col--) {
      uint64_t bit = (bits >> (row * 8 + col)) & (uint64_t)1;
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
// I tried it but it seems like it's not doing the same thing, I need to
// investigate
uint64_t reverse(uint64_t b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
         (b >> 48);
}

} // namespace bits
