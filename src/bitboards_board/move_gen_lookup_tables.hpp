#include <array>
#include <sys/types.h>

struct MoveGenLookupTables {
  std::array<u_int64_t, 64> bb_squares;
  std::array<u_int64_t, 64> bb_knight_moves;
};

MoveGenLookupTables create_lookup_tables();
