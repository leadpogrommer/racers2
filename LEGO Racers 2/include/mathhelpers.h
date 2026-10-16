#ifndef LEGORACERS2_MATHHELPERS_H
#define LEGORACERS2_MATHHELPERS_H

#include "decomp.h"
#include "types.h"

class SplineBasis {
public:
	LegoFloat m_basis[4][4];      // 0x00-0x3f: four rows of four basis values
	LegoFloat m_derivative[4][4]; // 0x40-0x7f: corresponding derivatives
	LegoU32 m_unk0x80;            // 0x80: basis mode
	LegoFloat m_unk0x84;          // 0x84: scalar used by other modes

	SplineBasis(LegoU32 p_mode, LegoFloat p_parameter);
	void FUN_004d1610();
	void FUN_004d17d0();
};

DECOMP_SIZE_ASSERT(SplineBasis, 0x88);

#endif // LEGORACERS2_MATHHELPERS_H
