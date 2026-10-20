#ifndef LR2_GLOBALPAUSE_H
#define LR2_GLOBALPAUSE_H

#include "game/objects/gamemanager.h"

struct PauseObserver;

struct PauseObserverDispatch {
	typedef void(__fastcall* Enter)(PauseObserver*, void*, LegoU32);
	typedef void(__fastcall* Leave)(PauseObserver*, void*);
	typedef void(__fastcall* Update)(PauseObserver*, void*, LegoFloat);
	undefined m_unk0x00[8];
	Enter m_enter;   // 0x08
	Leave m_leave;   // 0x0c
	Update m_update; // 0x10
};

struct PauseObserver {
	const PauseObserverDispatch* m_vtable;
};

// Working name from the CRT initializer's "Global Pause Mode" template.
class GlobalPauseController : public GameObject {
public:
	GlobalPauseController(GameObjectTemplate* p_template);
	void EnterPause();
	void LeavePause();
	void Update();
	void SetObserver(PauseObserver* p_observer);
	void ClearObserver();
	void SetDeadline(LegoFloat p_seconds);
	LegoBool32 IsBeforeDeadline();

	LegoU32 m_paused;          // 0x58
	LegoU32 m_enterRequested;  // 0x5c
	LegoU32 m_leaveRequested;  // 0x60
	LegoU32 m_observerMode;    // 0x64
	LegoU32 m_resumeInput;     // 0x68
	LegoFloat m_lastTime;      // 0x6c
	LegoFloat m_deadline;      // 0x70
	PauseObserver* m_observer; // 0x74
};

DECOMP_STATIC_ASSERT(offsetof(GlobalPauseController, m_paused) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(GlobalPauseController, m_observer) == 0x74);
DECOMP_SIZE_ASSERT(GlobalPauseController, 0x78);

extern GlobalPauseController& g_globalPauseController;

LegoFloat ReadSignedClockSeconds();

#endif // LR2_GLOBALPAUSE_H
