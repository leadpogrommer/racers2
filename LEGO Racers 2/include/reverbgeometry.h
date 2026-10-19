#ifndef LR2_REVERBGEOMETRY_H
#define LR2_REVERBGEOMETRY_H

#include "gamemanager.h"
#include "vector3.h"

struct ReverbBoundaryPlane : GameObject {
	Vector3 m_reference;       // 0x58
	Vector3 m_corners[4];      // 0x64
	Vector3 m_normal;          // 0x94
	LegoU32 m_mode;            // 0xa0
	LegoFloat m_radiusSquared; // 0xa4

	LegoBool32 IntersectSegment(const Vector3*, const Vector3*, const Vector3*, LegoFloat*);
	LegoBool32 ContainsPoint(const Vector3*) const;
};
DECOMP_SIZE_ASSERT(ReverbBoundaryPlane, 0xa8);
DECOMP_STATIC_ASSERT(offsetof(ReverbBoundaryPlane, m_normal) == 0x94);

// cReverbArea allocates 0x8c bytes, including eight inline plane pointers.
struct ReverbVolumeView : GameObject {
	LegoFloat m_firstAudioParameter;  // 0x58
	LegoFloat m_secondAudioParameter; // 0x5c
	LegoU32 m_unk0x60;
	LegoU32 m_underground;            // 0x64
	LegoS32 m_planeCount;             // 0x68
	ReverbBoundaryPlane* m_planes[8]; // 0x6c

	LegoBool32 ContainsPoint(const Vector3*, LegoU32) const;
	LegoBool32 IntersectSegment(const Vector3*, const Vector3*, const Vector3*, LegoFloat*);
};
DECOMP_SIZE_ASSERT(ReverbVolumeView, 0x8c);
DECOMP_STATIC_ASSERT(offsetof(ReverbVolumeView, m_planeCount) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(ReverbVolumeView, m_planes) == 0x6c);

#endif
