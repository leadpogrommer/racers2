#ifndef LEGORACERS2_SPLINE_H
#define LEGORACERS2_SPLINE_H

#include "decomp.h"
#include "mathhelpers.h"
#include "types.h"
#include "vector3.h"

struct SplineSegment {
	Vector3 m_vector0x00; // 0x00
	Vector3 m_vector0x0c; // 0x0c
	LegoFloat m_length;   // 0x18
	LegoU32 m_flags;      // 0x1c
};

DECOMP_SIZE_ASSERT(SplineSegment, 0x20);

class SplinePath {
public:
	LegoU8 m_unk0x00[0x58];
	SplineSegment* m_segments; // 0x58
	LegoS32 m_closed;          // 0x5c
	LegoS32 m_segmentCount;    // 0x60
	LegoU8 m_unk0x64[4];
	LegoFloat m_totalLength;       // 0x68
	LegoS32 m_cachedSegmentIndex;  // 0x6c
	LegoS32 m_cachedSampleIndex;   // 0x70
	Vector3 m_cachedPositions[16]; // 0x74-0x133
	LegoFloat m_cachedLengths[16]; // 0x134-0x173
	SplineBasis* m_basis;          // 0x174

	// Recovered member accesses end at 0x177; the full object extent is unknown.
	LegoFloat FUN_004d1a20();
	LegoFloat FUN_004d1a70();
	LegoFloat FUN_004d1aa0(LegoS32 p_segmentIndex, LegoFloat p_t, LegoFloat p_startT, LegoS32 p_storeLength);
	LegoFloat FUN_004d1d90(Vector3* p_result, LegoFloat p_distance, LegoS32* p_segmentIndex);
	Vector3* FUN_004d0f60(Vector3* p_result, LegoS32 p_segmentIndex, LegoFloat p_t);
	Vector3* FUN_004d11c0(Vector3* p_result, LegoS32 p_segmentIndex, LegoFloat p_t);
	Vector3* FUN_004d1390(Vector3* p_result, LegoS32 p_segmentIndex, LegoFloat p_t);
};

void FUN_004d1800(const Vector3* p_reference, Vector3* p_adjustable);

#endif // LEGORACERS2_SPLINE_H
