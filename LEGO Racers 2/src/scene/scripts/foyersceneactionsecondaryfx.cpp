#include "game/controller/gamecontroller.h"

// STUB: LEGORACERS2 0x0046ff90
void __fastcall InitializeFoyerSceneSecondaryFxTransition(
	void* p_manager,
	void*,
	LegoU32 p_startBits,
	LegoU32 p_endBits,
	LegoU32 p_durationBits
)
{
	LegoU8* record = (LegoU8*) p_manager;
	LegoU32 index = *(LegoU32*) (record + 0x680);
	if (index == 0x20) {
		return;
	}
	*(LegoU32*) (record + 0x680) = index + 1;
	record += index * 0x34;
	*(LegoU32*) record = 1;
	*(LegoU32*) (record + 0x10) = p_startBits;
	*(LegoU32*) (record + 0x14) = p_startBits;
	*(LegoU32*) (record + 0x1c) = p_endBits;
	*(LegoU32*) (record + 0x20) = p_endBits;
	*(LegoU32*) (record + 0x24) = 0;
	*(LegoU32*) (record + 0x28) = 0;
	*(LegoU32*) (record + 0x2c) = 0;
	*(LegoU32*) (record + 0x30) = p_durationBits;
}

#include "math/vector3.h"

#include <math.h>

extern LegoU32 __fastcall AddFoyerSceneSecondaryFxCircle(
	void* p_manager,
	void* p_unusedEdx,
	const Vector3* p_position,
	LegoFloat p_fade,
	LegoU32 p_color
);

// STUB: LEGORACERS2 0x00470630
void __fastcall DrawFoyerSceneCircles(void* p_manager, void*, const Vector3* p_center)
{
	const LegoFloat step = (3.1415927410125732f + 3.1415927410125732f) * 0.015625f;
	LegoFloat angle = 0.0f;
	for (LegoU32 i = 0; i < 0x40; ++i) {
		Vector3 position;
		position.m_x = sinf(angle) * 3.0f + p_center->m_x;
		position.m_y = p_center->m_y;
		position.m_z = cosf(angle) * 3.0f + p_center->m_z;
		AddFoyerSceneSecondaryFxCircle(p_manager, NULL, &position, 0.0f, 0x7f7f7f7f);
		angle += step;
	}
}
