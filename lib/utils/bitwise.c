#include "bitwise.h"

static const unsigned char bruijnMappingTableUInt[32] = 
{
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

inline unsigned char __attribute__((always_inline)) compute_lsb_pos(const unsigned int value)
{
    return bruijnMappingTableUInt[((value & (~value + 1)) * 0x077CB531U) >> 27];
}