#include "gamecameraregistry.h"

namespace
{
struct GameCameraRegistryView {
	undefined m_unk0x00[0x1cc];
	LegoU32 m_active;
};
DECOMP_STATIC_ASSERT(offsetof(GameCameraRegistryView, m_active) == 0x1cc);
} // namespace

// STUB: LEGORACERS2 0x004ab1c0
LegoS32 CountActiveGameCameras()
{
	LegoS32 count = 0;
	for (LegoU32 index = 0; index < (LegoU32) g_gameObjectCount; ++index) {
		GameObject* object = g_gameObjects[index];
		LegoS32 type = (LegoS32) (object->m_type & 0xffffff00);
		if (type >= 0x300) {
			if (type != 0x300) {
				return count;
			}
			if ((object->m_flags & 1) == 0 && ((GameCameraRegistryView*) object)->m_active != 0) {
				++count;
			}
		}
	}
	return count;
}

// STUB: LEGORACERS2 0x004ab220
GameObject* __fastcall FindActiveGameCamera(GameManager*, void*, LegoS32 p_index)
{
	LegoS32 count = 0;
	for (LegoU32 index = 0; index < (LegoU32) g_gameObjectCount; ++index) {
		GameObject* object = g_gameObjects[index];
		LegoS32 type = (LegoS32) (object->m_type & 0xffffff00);
		if (type >= 0x300) {
			if (type != 0x300) {
				return NULL;
			}
			if ((object->m_flags & 1) == 0 && ((GameCameraRegistryView*) object)->m_active != 0) {
				if (count == p_index) {
					return object;
				}
				++count;
			}
		}
	}
	return NULL;
}
