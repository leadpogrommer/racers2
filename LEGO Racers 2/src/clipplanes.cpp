#include "clipplanes.h"

// STUB: LEGORACERS2 0x004a3120
void __fastcall SetFoyerSceneTargetAnimation(void* p_target, void*, LegoU32 p_state)
{
	void* clipPlanes = *(void**) ((LegoU8*) p_target + 0x48);
	if (clipPlanes != NULL) {
		SetClipPlanesForFoyerTarget(clipPlanes, NULL, p_state);
	}
}

// FUNCTION: LEGORACERS2 0x004aa360
void __fastcall SetClipPlanesForFoyerTarget(void* p_target, void*, LegoU32 p_state)
{
	LegoU8* bytes = (LegoU8*) p_target;
	if (p_state == 1) {
		*(LegoFloat*) (bytes + 0xcc) = g_modelClipFar;
		*(LegoFloat*) (bytes + 0xc8) = g_modelClipNear;
		*(LegoFloat*) (bytes + 0xc4) = 1.0f / (g_modelClipFar - g_modelClipNear);
	}
	else {
		*(LegoU32*) (bytes + 0xc4) = 0;
		*(LegoU32*) (bytes + 0xc8) = 0x46000000;
		*(LegoU32*) (bytes + 0xcc) = 0x46000000;
	}
}

// GLOBAL: LEGORACERS2 0x00549c74
LegoFloat g_modelClipNear = 150.0f;
// GLOBAL: LEGORACERS2 0x00549c78
LegoFloat g_modelClipFar = 200.0f;
