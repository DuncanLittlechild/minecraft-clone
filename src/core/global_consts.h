#ifndef DL_TEMPLATE_GLOBALCONSTS_H
#define DL_TEMPLATE_GLOBALCONSTS_H
#include <cstdint>

// Different macros for the three uses of static
// first is for global static variables, which have internal linkage (local to the translation unit)
// second is for class static variables, shared by all objects of that class
// third is for local function static variables, which are store din static memory
#define LOCAL static
#define SHARED static
#define STATIC static

// These values are used to calculate chunk size.
// they must all be powers of 2
// CHUNKX and CHUNKZ must always have the same value
constexpr uint16_t CHUNKX {16};
constexpr uint16_t CHUNKY {256};
constexpr uint16_t CHUNKZ {16};
constexpr uint32_t CHUNKMAXBLOCKS {CHUNKY * CHUNKX * CHUNKZ};

static_assert(CHUNKX == CHUNKZ);

#endif