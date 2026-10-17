#ifndef LR2_SCALARUTIL_H
#define LR2_SCALARUTIL_H

#include "types.h"

LegoS32 RoundUpToPowerOfTwo(LegoS32 p_value);
LegoS32 CountTrailingZeroBits(LegoS32 p_value);
void BuildSineTable(LegoFloat* p_output, LegoS32 p_count);

#endif // LR2_SCALARUTIL_H
