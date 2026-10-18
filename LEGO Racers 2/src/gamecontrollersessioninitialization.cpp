#include "gamecontrollerinitialization.h"
#include "gamecontrollersession.h"

struct ControllerSessionTail {
	LegoU32 m_unk0x9b58;
	LegoU32 m_unk0x9b5c;
};

// FUNCTION: LEGORACERS2 0x0040abc0
void __fastcall ResetGameControllerSession(GameController* p_controller, void*)
{
	GameControllerSession* session = (GameControllerSession*) ((LegoU8*) p_controller + 0x80ec);
	session->m_track = 1;
	session->m_unk0x14 = 1;
	session->m_unk0x18 = 1;
	session->m_unk0x00 = 0;
	session->m_mode = 0;
	session->m_selectedMode = 0;
	session->m_unk0x20 = 1.0f;
	session->m_unk0x24 = 0;
	for (LegoU32 i = 0; i < 8; ++i) {
		session->m_unk0x28[i] = 0;
	}
	session->m_unk0x48 = 0;
	session->m_unk0x1c = 0;
	ControllerSessionTail* tail = (ControllerSessionTail*) ((LegoU8*) p_controller + 0x9b58);
	tail->m_unk0x9b58 = 0;
	tail->m_unk0x9b5c = 0;
}
