#ifndef LR2_INPUTCONTROLLERMANAGER_H
#define LR2_INPUTCONTROLLERMANAGER_H

#include "inputcontroller.h"
#include "inputdevice.h"
#include "multimediatimer.h"

#include <stddef.h>

class InputControllerManager;

struct ControllerManagerDispatch {
	typedef LegoBool32(__fastcall* Initialize)(InputControllerManager*, void*);
	typedef void(__fastcall* Shutdown)(InputControllerManager*, void*);
	typedef HANDLE(__fastcall* Acquire)(InputControllerManager*, void*);
	typedef void(__fastcall* Release)(InputControllerManager*, void*, HANDLE);
	Initialize m_initialize;
	Shutdown m_shutdown;
	Acquire m_acquire;
	Release m_release;
};
DECOMP_SIZE_ASSERT(ControllerManagerDispatch, 0x10);

struct ControllerTimingState {
	LegoU32 m_absoluteSeconds;
	LegoU32 m_absoluteMicroseconds;
	LegoU32 m_relativeSeconds;
	LegoU32 m_relativeMicroseconds;
	LegoU32 m_absoluteFrame;
	LegoU32 m_relativeFrame;
};
DECOMP_SIZE_ASSERT(ControllerTimingState, 0x18);

class InputControllerManager {
public:
	void InitializeTiming(LegoU32 p_frequency, LegoU32 p_pollMultiplier);
	LegoBool32 ResetRecording(LegoU32 p_mode);
	void Shutdown();
	void SetPaused(LegoU32 p_paused);
	InputController* FindOrCreateController(
		LegoU32 p_ownerKey,
		LegoU32 p_index,
		LegoBool32 p_create,
		LegoBool32 p_record
	);
	void RebindController(LegoU32 p_ownerKey, LegoU32 p_newOwnerKey, LegoU32 p_index, LegoU32 p_newIndex);
	void ConfigureController(LegoU32 p_ownerKey, LegoU32 p_index, LegoBool32 p_record, LegoBool32 p_defaults);
	void AppendControllerBinding(LegoU32 p_ownerKey, LegoU32 p_index, const ControllerBinding* p_binding);
	void GetTimingState(ControllerTimingState* p_state);
	void GetElapsedTime(LegoU32* p_seconds, LegoU32* p_microseconds, LegoU32* p_frame);
	void GetInputRequestElapsedTime(LegoU32* p_seconds, LegoU32* p_microseconds);
	LegoU32 GetCurrentFrame();
	LegoBool32 PollInput(LegoBool32 p_force);
	LegoBool32 PollController(InputController* p_controller, LegoU32 p_frames);
	void GetControllerInputForOwner(
		LegoU32 p_ownerKey,
		LegoU32* p_output,
		LegoU32 p_controllerIndex,
		LegoU32 p_forceLiveInput
	);
	LegoBool32 InitializePolling();
	void StopPolling();
	HANDLE AcquirePollMutex();
	void ReleasePollMutex(HANDLE p_mutex);

	const ControllerManagerDispatch* m_vtable; // 0x00
	LegoS32 m_currentFrame;                    // 0x04
	LegoU32 m_frequency;                       // 0x08
	LegoU32 m_pollMultiplier;                  // 0x0c
	LegoS32 m_controllerCount;                 // 0x10
	LegoU32 m_unk0x14;
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
	LegoFloat m_axisScale0; // 0x20
	LegoFloat m_axisScale1; // 0x24
	LegoU32 m_unk0x28;
	InputController* m_controllers[12]; // 0x2c
	InputDeviceRecord* m_keyboard;      // 0x5c
	InputDeviceRecord* m_mouse;         // 0x60
	LegoBool32 m_paused;                // 0x64
	LegoU32 m_unk0x68;
	LegoU32 m_unk0x6c;
	LegoU32 m_unk0x70;
	LegoU32 m_pausedSeconds;      // 0x74
	LegoU32 m_pausedMicroseconds; // 0x78
	LegoU32 m_unk0x7c;
	LegoU32 m_pausedFrame;        // 0x80
	LegoU32 m_originSeconds;      // 0x84
	LegoU32 m_originMicroseconds; // 0x88
	LegoU32 m_unk0x8c;
	LegoU32 m_unk0x90;
	LegoU32 m_originFrame; // 0x94
	LegoU32 m_unk0x98[0x34 / 4];
	MMRESULT m_timerId; // 0xcc
	LegoU32 m_unk0xd0[2];
	HANDLE m_pollMutex; // 0xd8
};

// The visible layout extends through the mutex handle; no larger size is claimed.
DECOMP_STATIC_ASSERT(offsetof(InputControllerManager, m_controllers) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(InputControllerManager, m_keyboard) == 0x5c);
DECOMP_STATIC_ASSERT(offsetof(InputControllerManager, m_originFrame) == 0x94);
DECOMP_STATIC_ASSERT(offsetof(InputControllerManager, m_timerId) == 0xcc);
DECOMP_STATIC_ASSERT(offsetof(InputControllerManager, m_pollMutex) == 0xd8);

extern InputControllerManager g_inputControllerManagerStorage;
extern InputControllerManager* g_inputControllerManager;
extern const ControllerManagerDispatch g_controllerManagerDispatch;
InputDeviceRecord* __fastcall GetControllerInputDevice(InputController* p_controller, void*);

#endif // LR2_INPUTCONTROLLERMANAGER_H
