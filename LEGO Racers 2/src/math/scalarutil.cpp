#include "math/scalarutil.h"

#include <math.h>

// FUNCTION: LEGORACERS2 0x004a5dc0
LegoS32 RoundUpToPowerOfTwo(LegoS32 p_value)
{
	LegoS32 shift = 0;
	for (; shift < 32; ++shift) {
		if (p_value <= (LegoS32) (1u << shift)) {
			return (LegoS32) (1u << shift);
		}
	}
	return 0;
}

// FUNCTION: LEGORACERS2 0x004a5df0
LegoS32 CountTrailingZeroBits(LegoS32 p_value)
{
	LegoS32 count = 0;

	if (p_value != 0 && (p_value & 1) == 0) {
		do {
			p_value >>= 1;
			++count;
		} while ((p_value & 1) == 0);
	}
	return count;
}

// FUNCTION: LEGORACERS2 0x004a5e10
void BuildSineTable(LegoFloat* p_output, LegoS32 p_count)
{
	LegoFloat reciprocal = 1.0f / p_count;
	for (LegoS32 index = 0; index < p_count; ++index) {
		LegoFloat angle = index * 3.1415927f;
		angle *= reciprocal;
		p_output[index] = (LegoFloat) sin(angle + angle);
	}
}
