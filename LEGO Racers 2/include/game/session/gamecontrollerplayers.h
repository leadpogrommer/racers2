#ifndef LR2_GAMECONTROLLERPLAYERS_H
#define LR2_GAMECONTROLLERPLAYERS_H

#include "core/strings/charbuffer128.h"
#include "game/character/characterpartpreset.h"
#include "game/controller/gamecontroller.h"

struct StartupPlayerSettings {
	LegoU32 m_index;
	LegoS32 m_unk0x04;
	LegoU32 m_unk0x08;
	LegoU32 m_unk0x0c;
	LegoS32 m_unk0x10;
	LegoU32 m_unk0x14;
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
	GameControllerItemBlock m_carConfiguration;
	Vector3 m_unk0x314;
	CharacterPartPreset* m_partPreset;
	LegoU32 m_unk0x324[4];
	LegoU32 m_unk0x334;
	LegoU32 m_unk0x338;
	LegoU32 m_unk0x33c;
	LegoU32 m_unk0x340;
};
DECOMP_STATIC_ASSERT(offsetof(StartupPlayerSettings, m_unk0x314) == 0x314);
DECOMP_STATIC_ASSERT(offsetof(StartupPlayerSettings, m_unk0x338) == 0x338);
DECOMP_SIZE_ASSERT(StartupPlayerSettings, 0x344);

// Retail 0x0053ed7c: complete shared catalogue, first eight seed player cars.
extern const LegoChar* g_defaultPlayerCarPaths[46];

void LoadDefaultPlayerCar(const CharBuffer128* p_path, GameControllerItemBlock* p_configuration);

#endif
