#include "core/application/applicationmessages.h"

#include "game/controller/gamecontroller.h"

extern LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];

// FUNCTION: LEGORACERS2 0x00493550
LegoBool32 WINAPI FilterApplicationWindowMessage(HWND p_window, UINT p_message, WPARAM p_wParam, LPARAM p_lParam)
{
	(void) p_window;
	(void) p_lParam;

	if (p_message < 7) {
		return TRUE;
	}
	if (p_message <= 8) {
		return TRUE;
	}
	if (p_message != WM_CHAR) {
		return TRUE;
	}

	GameController* controller = (GameController*) g_gameControllerStorage;
	GameApplicationMessageContextView* context = (GameApplicationMessageContextView*) controller->m_networkApp;
	if (context == NULL || (context->m_inputMode != 2 && context->m_alternateInputMode != 4) ||
		context->m_textInputTarget == NULL) {
		return TRUE;
	}

	context->m_characterQueue.QueueCharacter((LegoU32) p_wParam);
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x00494f40
void ApplicationCharacterQueue::QueueCharacter(LegoU32 p_character)
{
	if (m_busy != 0) {
		return;
	}

	LegoU32 nextWriteIndex = m_writeIndex + 1;
	if (nextWriteIndex == 64) {
		nextWriteIndex = 0;
	}
	if (nextWriteIndex == m_readIndex) {
		return;
	}

	m_busy = 1;
	m_characters[m_writeIndex] = (LegoU16) p_character;
	++m_writeIndex;
	if (m_writeIndex == 64) {
		m_writeIndex = 0;
	}
	m_busy = 0;
}
