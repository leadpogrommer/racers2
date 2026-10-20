#include "game/controller/gamecontroller.h"

// FUNCTION: LEGORACERS2 0x004071a0
GameControllerScore* GameControllerScore::Initialize()
{
	m_name.Initialize();
	m_value = -1.0f;
	return this;
}

// STUB: LEGORACERS2 0x004071f0
void GameControllerPairGroups::Initialize()
{
	LegoU32 i;
	for (i = 0; i < 2; ++i) {
		m_pairs[i].m_first = 0;
		m_pairs[i].m_second = 0;
	}
	for (i = 0; i < 9; ++i) {
		m_firstGroups[i].m_first = 0;
		m_firstGroups[i].m_second = 0;
	}
	for (i = 0; i < 9; ++i) {
		m_secondGroups[i].m_first = 0;
		m_secondGroups[i].m_second = 0;
	}
}

// FUNCTION: LEGORACERS2 0x00409190 FOLDED
GameControllerItem* GameControllerItem::Initialize()
{
	return this;
}

// FUNCTION: LEGORACERS2 0x0040aef0
GameControllerName* GameControllerName::Initialize()
{
	for (LegoU32 i = 0; i < 8; ++i) {
		m_text[i] = '-';
	}
	m_text[8] = 0;
	return this;
}

// FUNCTION: LEGORACERS2 0x0040af10
GameControllerName* GameControllerName::SetNarrowText(const LegoChar* p_text)
{
	LegoU16* output = m_text;
	do {
		*output++ = (LegoS16) *p_text;
	} while (*p_text++ != 0);
	return this;
}

// STUB: LEGORACERS2 0x0040af30
GameControllerName* GameControllerName::SetWideText(const LegoU16* p_text)
{
	LegoU16* output = m_text;
	do {
		*output++ = *p_text;
	} while (*p_text++ != 0);
	return this;
}

// STUB: LEGORACERS2 0x0040b000
void GameControllerSelection::Initialize()
{
	m_values[0] = 0;
	m_values[1] = 0;
	m_values[2] = -1;
	m_values[3] = -1;
	m_values[4] = -1;
	m_values[5] = -1;
	m_values[6] = -1;
	m_values[7] = 0;
}

// FUNCTION: LEGORACERS2 0x0040b340 FOLDED
void __fastcall InitializeGameControllerWord(LegoU32* p_word, void*)
{
	*p_word = 0;
}
