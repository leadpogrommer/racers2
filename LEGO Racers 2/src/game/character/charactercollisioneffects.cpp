#include "game/character/charactercollisioneffects.h"

#include "game/controller/gamecontroller.h"
#include "game/state/gamerandom.h"
#include "render/core/renderer.h"
#include "scene/scripts/foyerscenecirclepool.h"

#include <string.h>

struct CharacterCollisionParticleManager {
	const void* m_vtable;
	void* m_records[512];
	LegoU32 m_removedCount;
	LegoU32 m_nextIndex;
	LegoS32 m_texture;
};
DECOMP_STATIC_ASSERT(offsetof(CharacterCollisionParticleManager, m_removedCount) == 0x804);
DECOMP_STATIC_ASSERT(offsetof(CharacterCollisionParticleManager, m_texture) == 0x80c);

static LegoFloat NextCharacterImpactRandom()
{
	GameFloatRandom* stream = &GetGameRandomState()->m_floatStreams[1];
	return GetFoyerSceneRandomUnit(stream, NULL);
}

static void __fastcall AddCharacterCollisionParticle(
	CharacterCollisionParticleManager*,
	void*,
	const Vector3*,
	const LegoU32*,
	LegoFloat,
	LegoU32
);

// STUB: LEGORACERS2 0x004403f0
void __fastcall EmitCharacterCollisionDust(
	void*,
	void*,
	const Vector3* p_position,
	LegoFloat p_speed,
	LegoU32 p_color,
	LegoFloat p_brightness
)
{
	if (!(p_speed >= 10.0f)) {
		return;
	}
	if (p_speed > 30.0f) {
		p_speed = 30.0f;
	}
	LegoFloat scale = (p_speed - 10.0f) * 0.05f * (NextCharacterImpactRandom() * 0.8f + 0.6f) * 0.6f + 0.4f;
	if (p_brightness > 0.5f) {
		p_brightness = 0.5f;
	}
	p_brightness = p_brightness * 0.4f * 255.0f;
	LegoS32 multiplier;
	__asm {
		fld p_brightness
		fistp multiplier
	}
	if (multiplier < 0) multiplier = 0;
	else if (multiplier > 255) multiplier = 255;
	LegoU8* channels = (LegoU8*) &p_color;
	channels[2] = (channels[2] * multiplier) >> 8;
	channels[1] = (channels[1] * multiplier) >> 8;
	channels[0] = (channels[0] * multiplier) >> 8;
	channels[3] = (channels[3] * multiplier) >> 8;
	Vector3 position = *p_position;
	position.m_x = (NextCharacterImpactRandom() * 0.5f - 0.25f) + position.m_x;
	position.m_z = (NextCharacterImpactRandom() * 0.5f - 0.25f) + position.m_z;
	position.m_y = scale * 0.5f + position.m_y;
	void* effects = (void*) g_gameController.m_unk0xa4;
	CharacterCollisionParticleManager* manager = *(CharacterCollisionParticleManager**) ((LegoU8*) effects + 0x78);
	AddCharacterCollisionParticle(manager, NULL, &position, &p_color, scale, 0);
}

static LegoU32 __fastcall RemoveCharacterCollisionParticle(CharacterCollisionParticleManager*, void*, LegoU32);
static void* __fastcall AllocateCharacterCollisionParticle(CharacterCollisionParticleManager*, void*);

// STUB: LEGORACERS2 0x004707c0
static void __fastcall ClearCharacterCollisionParticles(CharacterCollisionParticleManager* p_manager, void*)
{
	p_manager->m_removedCount = 0;
	for (LegoU32 i = 0; i < 512; ++i) {
		if (p_manager->m_records[i] != NULL) {
			RemoveCharacterCollisionParticle(p_manager, NULL, i);
		}
		p_manager->m_records[i] = NULL;
	}
	p_manager->m_removedCount = 0;
	p_manager->m_nextIndex = 0;
}

// STUB: LEGORACERS2 0x00470ad0
static void __fastcall AddCharacterCollisionParticle(
	CharacterCollisionParticleManager* p_manager,
	void*,
	const Vector3* p_position,
	const LegoU32* p_color,
	LegoFloat p_scale,
	LegoU32
)
{
	if (g_displayRenderer->m_unk0x4d0 <= 3) {
		return;
	}
	LegoU8* record = (LegoU8*) AllocateCharacterCollisionParticle(p_manager, NULL);
	if (record == NULL) {
		return;
	}
	*(LegoU32*) (record + 0x1c) = *p_color;
	*(LegoU32*) (record + 0x0c) = *p_color;
	*(LegoFloat*) (record + 0x38) = p_scale;
	*(LegoFloat*) (record + 0x3c) = p_scale;
	*(LegoFloat*) (record + 0x34) = 0.25f;
	*(Vector3*) (record + 0x10) = *p_position;
	*(LegoU32*) (record + 0x4c) = 0;
	*(LegoU32*) (record + 0x48) = 2;
	LegoU32 values[5];
	values[0] = p_manager->m_texture;
	values[1] = values[2] = 0;
	values[3] = values[4] = 0x3f800000;
	memcpy(record + 0x20, values, sizeof(values));
}

// FUNCTION: LEGORACERS2 0x00470ba0
static LegoBool32 __fastcall IsCharacterCollisionParticleActive(
	CharacterCollisionParticleManager*,
	void*,
	void* p_record
)
{
	return p_record != NULL;
}

// STUB: LEGORACERS2 0x00470bb0
static LegoU32 __fastcall RemoveCharacterCollisionParticle(
	CharacterCollisionParticleManager* p_manager,
	void*,
	LegoU32 p_index
)
{
	if (!IsCharacterCollisionParticleActive(p_manager, NULL, p_manager->m_records[p_index])) {
		return 0;
	}
	g_foyerSceneCirclePool.FreeRecord(p_manager->m_records[p_index]);
	p_manager->m_records[p_index] = NULL;
	++p_manager->m_removedCount;
	return 1;
}

// STUB: LEGORACERS2 0x00470c00
static void* __fastcall AllocateCharacterCollisionParticle(CharacterCollisionParticleManager* p_manager, void*)
{
	void* record = g_foyerSceneCirclePool.AllocateRecord();
	if (record == NULL) {
		return NULL;
	}
	if (p_manager->m_nextIndex >= 0xffffffff) {
		ClearCharacterCollisionParticles(p_manager, NULL);
	}
	LegoU32 index = p_manager->m_nextIndex & 0x1ff;
	++p_manager->m_nextIndex;
	if (IsCharacterCollisionParticleActive(p_manager, NULL, p_manager->m_records[index])) {
		RemoveCharacterCollisionParticle(p_manager, NULL, index);
	}
	p_manager->m_records[index] = record;
	return record;
}
