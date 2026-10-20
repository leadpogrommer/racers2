#include "frontend/secondary/secondarymanagerleases.h"

#include "game/state/gamerandom.h"

struct MenuPreviewCameraLeaseView {
	undefined m_prefix[0x1c0];
	LegoU32 m_enabled;
};
DECOMP_STATIC_ASSERT(offsetof(MenuPreviewCameraLeaseView, m_enabled) == 0x1c0);

static LegoS32 GenerateMenuPreviewLeaseKey()
{
	GameIntegerRandom& stream = GetGameRandomState()->m_integerStreams[1];
	stream.m_state = stream.m_multiplier * stream.m_state + stream.m_increment;
	return stream.m_state == 0xffffffff ? 0 : (LegoS32) stream.m_state;
}

// STUB: LEGORACERS2 0x004290b0
LegoS32 __fastcall AcquireMenuPreviewCamera(SecondaryManager* p_manager, void*, void** p_output)
{
	for (LegoS32 index = 0; index < 3; ++index) {
		if (p_manager->m_resourceIds[index] == -1) {
			p_manager->m_resourceIds[index] = GenerateMenuPreviewLeaseKey();
			*p_output = p_manager->m_resourcePointers[index];
			return p_manager->m_resourceIds[index];
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x00429130
void __fastcall ReleaseMenuPreviewCamera(SecondaryManager* p_manager, void*, void** p_output, LegoS32 p_key)
{
	for (LegoS32 index = 0; index < 3; ++index) {
		if (p_manager->m_resourceIds[index] == p_key) {
			p_manager->m_resourceIds[index] = -1;
			*p_output = NULL;
			((MenuPreviewCameraLeaseView*) p_manager->m_resourcePointers[index])->m_enabled = 0;
			return;
		}
	}
}

// STUB: LEGORACERS2 0x00429180
LegoS32 __fastcall AcquireMenuPreviewCharacter(SecondaryManager* p_manager, void*, void** p_output)
{
	if (p_manager->m_unk0x130 != -1) {
		return -1;
	}
	p_manager->m_unk0x130 = GenerateMenuPreviewLeaseKey();
	*p_output = (void*) p_manager->m_unk0x12c;
	return p_manager->m_unk0x130;
}

// STUB: LEGORACERS2 0x00429220
LegoS32 __fastcall AcquireMenuPreviewAmbient(SecondaryManager* p_manager, void*, void** p_output)
{
	if (p_manager->m_unk0x138 != -1) {
		return -1;
	}
	p_manager->m_unk0x138 = GenerateMenuPreviewLeaseKey();
	*p_output = p_manager->m_unk0x134;
	return p_manager->m_unk0x138;
}

// FUNCTION: LEGORACERS2 0x00429280
void __fastcall ReleaseMenuPreviewAmbient(SecondaryManager* p_manager, void*, void** p_output, LegoS32)
{
	p_manager->m_unk0x138 = -1;
	*p_output = NULL;
}

// STUB: LEGORACERS2 0x004292a0
LegoS32 __fastcall AcquireMenuPreviewLight(SecondaryManager* p_manager, void*, void** p_output)
{
	for (LegoS32 index = 0; index < 3; ++index) {
		if (p_manager->m_moreResourceIds[index] == -1) {
			p_manager->m_moreResourceIds[index] = GenerateMenuPreviewLeaseKey();
			*p_output = p_manager->m_moreResourcePointers[index];
			return p_manager->m_moreResourceIds[index];
		}
	}
	return -1;
}
