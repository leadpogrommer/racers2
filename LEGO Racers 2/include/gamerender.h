#ifndef LR2_GAMERENDER_H
#define LR2_GAMERENDER_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

LegoU32* __stdcall BuildAnimationHueColor(LegoU32* p_color, LegoS32 p_degrees, LegoS32 p_saturation, LegoS32 p_value);

struct GameObjectComponentDispatch {
	typedef void*(__fastcall* Transform)(void*, void*);
	typedef void(__fastcall* Update)(void*, void*);
	typedef void(__fastcall* GetTransforms)(void*, void*, void*, void*);

	undefined m_unk0x00[0x14];
	Transform m_getPosition; // 0x14
	undefined m_unk0x18[4];
	Transform m_getOrientation; // 0x1c
	undefined m_unk0x20[0x20];
	Update m_finishTransform;      // 0x40
	GetTransforms m_getTransforms; // 0x44
};

DECOMP_STATIC_ASSERT(offsetof(GameObjectComponentDispatch, m_getPosition) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(GameObjectComponentDispatch, m_getOrientation) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(GameObjectComponentDispatch, m_getTransforms) == 0x44);

#endif // LR2_GAMERENDER_H
