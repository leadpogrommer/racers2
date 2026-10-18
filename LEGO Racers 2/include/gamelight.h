#ifndef LR2_GAMELIGHT_H
#define LR2_GAMELIGHT_H

#include "directx8.h"
#include "gamemanager.h"
#include "vector3.h"

struct GameLightProperties {
	D3DCOLORVALUE m_ambient;
	D3DCOLORVALUE m_diffuse;
	D3DCOLORVALUE m_specular;
	LegoS32 m_enabled;
	LegoU32 m_unk0x34;
	LegoS32 m_lightIndex;
	void SetEnabled(LegoS32 p_enabled);
};
DECOMP_SIZE_ASSERT(GameLightProperties, 0x3c);
DECOMP_STATIC_ASSERT(offsetof(GameLightProperties, m_enabled) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(GameLightProperties, m_lightIndex) == 0x38);

// The observed prefix extends through the direction, without a complete
// allocation-size claim for the game object.
struct GameDirectionalLight : GameObject {
	GameLightProperties m_properties;
	Vector3 m_direction;
	void UpdateRendererLight();
};
DECOMP_STATIC_ASSERT(offsetof(GameDirectionalLight, m_properties) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(GameDirectionalLight, m_direction) == 0x94);

struct GameAmbientLight {
	undefined m_prefix[0x8c];
	LegoU8 m_color[4];
	void ApplyToRenderer();
};
DECOMP_STATIC_ASSERT(offsetof(GameAmbientLight, m_color) == 0x8c);

void __cdecl CopyGameLightProperties(D3DLIGHT8*, const GameLightProperties*);

#endif
