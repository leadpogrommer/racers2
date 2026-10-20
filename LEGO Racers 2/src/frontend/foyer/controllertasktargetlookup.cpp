#include "game/controller/gamecontroller.h"
#include "types.h"

struct ControllerTaskTargetIndexEntry {
	GameObject* m_object;
	GameObject* m_target;
};

struct ControllerTaskTargetIndexView {
	undefined m_gap0x00[0x19c];
	LegoU32 m_count;
	ControllerTaskTargetIndexEntry m_entries[1];
};

struct ControllerTaskTargetStateView {
	undefined m_gap0x00[0xdc];
	LegoU32 m_state;
	undefined m_gap0xe0[0xb8];
	LegoFloat m_taskType6Value;  // +0x198
	LegoFloat m_taskType10Value; // +0x19c
	LegoFloat m_taskType9Value;  // +0x1a0
	LegoFloat m_taskType5Value;  // +0x1a4
};

static GameObject* __fastcall FindActiveControllerTaskTarget(void*, void*, GameObject*);

// STUB: LEGORACERS2 0x0044a870
LegoU32 __fastcall GetControllerTaskTypeForTarget(GameObject* p_target, void*)
{
	GameObject* index = *(GameObject**) ((LegoU8*) &g_gameController + 0xac);
	GameObject* owner = FindActiveControllerTaskTarget(index, NULL, p_target);
	ControllerTaskTargetStateView* state = (ControllerTaskTargetStateView*) p_target;
	if (owner != NULL && *(LegoU32*) ((LegoU8*) owner + 0x180) == 7) {
		return 0;
	}
	if (state->m_taskType10Value > 0.0f) {
		return 10;
	}
	if (state->m_taskType9Value > 0.0f) {
		return 9;
	}
	if (state->m_taskType6Value < 0.0f || state->m_taskType6Value > 0.0f) {
		return 6;
	}
	if (state->m_taskType5Value > 0.0f) {
		return 5;
	}
	if (state->m_state == 2) {
		return 3;
	}
	return *(LegoU32*) (*(LegoU32*) ((LegoU8*) p_target + 0x120) + 0x3c) == 1 ? 7 : 4;
}

// STUB: LEGORACERS2 0x00490130
static GameObject* __fastcall FindActiveControllerTaskTarget(void* p_index, void*, GameObject* p_target)
{
	ControllerTaskTargetIndexView* index = (ControllerTaskTargetIndexView*) p_index;
	LegoU32 i;
	for (i = 0; i < index->m_count; ++i) {
		ControllerTaskTargetIndexEntry* entry = &index->m_entries[i];
		if (*(LegoU32*) ((LegoU8*) entry->m_object + 0x184) != 4 && entry->m_target == p_target) {
			return entry->m_object;
		}
	}
	return NULL;
}
