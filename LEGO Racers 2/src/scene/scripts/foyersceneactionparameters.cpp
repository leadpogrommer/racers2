#include "scene/scripts/foyerscene.h"

void __fastcall SetFoyerSceneActionParameters(void*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
void __fastcall ClearFoyerSceneActionParameters(void*, void*);

// STUB: LEGORACERS2 0x00480140
void __fastcall ApplyFoyerSceneActionParameters(
	FoyerSceneResource* p_resource,
	void*,
	LegoU32 p_index,
	LegoU32 p_source,
	LegoU32 p_target,
	LegoU32 p_active,
	LegoFloat p_scale
)
{
	if (p_scale != 0.0f) {
		p_resource->m_animationValues[p_index] = p_scale;
		SetFoyerSceneActionParameters(
			p_resource->m_children[p_index],
			NULL,
			p_source,
			p_target,
			p_active,
			*(LegoU32*) &(p_scale *= p_resource->m_animationMode)
		);
	}
	else {
		ClearFoyerSceneActionParameters(p_resource->m_children[p_index], NULL);
	}
}

// STUB: LEGORACERS2 0x004d7270
void __fastcall SetFoyerSceneActionParameters(
	void* p_transition,
	void*,
	LegoU32 p_source,
	LegoU32 p_target,
	LegoU32 p_active,
	LegoU32 p_scaleBits
)
{
	LegoU8* transition = (LegoU8*) p_transition;
	*(LegoU32*) (transition + 0x9c) = p_active != 0 ? 4 : 0;
	*(LegoU32*) (transition + 0x94) = p_scaleBits;
	*(LegoU32*) (transition + 0x88) = p_source;
	*(LegoU32*) (transition + 0x8c) = p_target;

	LegoU8* target = (LegoU8*) p_target;
	void* nested = target + 0x58;
	void** table = *(void***) nested;
	LegoU32 enabled = ((LegoU32(__fastcall*)(void*, void*, LegoU32)) table[0])(nested, NULL, 0) != 0;
	LegoU32* flags = (LegoU32*) (target + 0x64);
	*flags = (*flags & ~1) | ((*flags ^ enabled) & 1);
	*(LegoFloat*) (transition + 0x90) = *(LegoFloat*) ((LegoU8*) p_source + 0x68);
	*(LegoU32*) (transition + 0x98) = 0;
}

// STUB: LEGORACERS2 0x004d7300
void __fastcall ClearFoyerSceneActionParameters(void* p_transition, void*)
{
	LegoU8* transition = (LegoU8*) p_transition;
	void* target = *(void**) (transition + 0x8c);
	if (target != NULL) {
		*(LegoU32*) ((LegoU8*) target + 0x64) &= ~1;
	}
	*(LegoU32*) (transition + 0x9c) |= 0x80;
}
