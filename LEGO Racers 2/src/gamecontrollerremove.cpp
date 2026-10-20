#include "bitmapfontset.h"
#include "gamecontroller.h"
#include "globaloverlaymanager.h"
#include "localizedstringtable.h"
#include "memoryheap.h"

void UnregisterGameControllerCallbacks();

static void ReleaseControllerChild(GameObject** p_child)
{
	GameObject* child = *p_child;
	if (child == NULL) {
		return;
	}

	child->m_vtable->m_remove(child, NULL);
	child = *p_child;
	if (child != NULL) {
		child->m_vtable->m_destroy(child, NULL, 1);
	}
	*p_child = NULL;
}

struct ControllerAuxiliaryDispatch {
	void* m_slot0;
	void(__fastcall* m_release)(void*, void*);
};

// STUB: LEGORACERS2 0x00407f40
void GameController::Remove()
{
	if (m_unk0xd8 != NULL) {
		LegoU32* values = (LegoU32*) m_unk0xd8;
		values[0] = 0;
		values[0xbb] = 0;
		FreeHeapMemory(values);
	}

	UnregisterGameControllerCallbacks();

	ReleaseControllerChild(&m_screenPrinter);

	if (m_unk0xd4 != NULL) {
		FreeHeapMemory(m_unk0xd4);
		m_unk0xd4 = NULL;
	}

	for (LegoU32 index = 0; index < 8; ++index) {
		if (m_playerProfiles[index] != NULL) {
			FreeHeapMemory(m_playerProfiles[index]);
			m_playerProfiles[index] = NULL;
		}
	}

	ReleaseControllerChild(&m_unk0xc8);
	ReleaseControllerChild(&m_networkApp);

	if (m_unk0xb4 != NULL) {
		ControllerAuxiliaryDispatch* dispatch = *(ControllerAuxiliaryDispatch**) m_unk0xb4;
		dispatch->m_release(m_unk0xb4, NULL);
		FreeHeapMemory(m_unk0xb4);
		m_unk0xb4 = NULL;
	}

	ReleaseControllerChild(&m_menuManager);
	ReleaseControllerChild(&m_secondaryFxManager);
	ReleaseControllerChild(&m_npcTextManager);
	ReleaseControllerChild(&m_textBoxManager);
	ReleaseControllerChild(&m_foyerManager);
	ReleaseControllerChild(&m_battleManager);
	ReleaseControllerChild(&m_raceManager);
	ReleaseControllerChild(&m_secondaryManager);

	if (m_unk0x8c != NULL) {
		FreeHeapMemory(m_unk0x8c);
		m_unk0x8c = NULL;
	}
	if (m_unk0x90 != NULL) {
		FreeHeapMemory(m_unk0x90);
		m_unk0x90 = NULL;
	}

	ReleaseControllerChild(&m_bodypartManager);

	if (m_unk0x70 != NULL) {
		FreeHeapMemory(m_unk0x70);
		m_unk0x70 = NULL;
	}

	ReleaseControllerChild(&m_pauseDetector);

	if (m_fontSet != NULL) {
		m_fontSet->~BitmapFontSet();
		FreeHeapMemory(m_fontSet);
	}

	if (m_localizedStrings != NULL) {
		if (g_globalOverlayManager.m_localizedStrings == m_localizedStrings) {
			g_globalOverlayManager.m_localizedStrings = NULL;
		}
		m_localizedStrings->Release();
		FreeHeapMemory(m_localizedStrings);
		m_localizedStrings = NULL;
	}

	GameObject::Remove();
}
