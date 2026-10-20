#include "input/controller/inputcontrollermanager.h"

#include "input/service/inputdeviceservice.h"
#include "memory/memoryheap.h"
#include "platform/timing/systemclock.h"

#include <new.h>

// GLOBAL: LEGORACERS2 0x00679d48
InputControllerManager g_inputControllerManagerStorage;

// GLOBAL: LEGORACERS2 0x0054a330
InputControllerManager* g_inputControllerManager = &g_inputControllerManagerStorage;

// STUB: LEGORACERS2 0x004b7c50
void InputControllerManager::InitializeTiming(LegoU32 p_frequency, LegoU32 p_pollMultiplier)
{
	m_pollMultiplier = p_pollMultiplier;
	m_frequency = p_frequency;
	m_unk0x1c = 0;
	m_vtable->m_initialize(this, NULL);
}

// STUB: LEGORACERS2 0x004b7c70
LegoBool32 InputControllerManager::ResetRecording(LegoU32 p_mode)
{
	HANDLE mutex = m_vtable->m_acquire(this, NULL);
	while (mutex == NULL) {
		mutex = m_vtable->m_acquire(this, NULL);
	}
	m_paused = 0;
	m_originFrame = 0;
	m_originSeconds = 0;
	m_originMicroseconds = 0;
	m_unk0x18 = 0;
	LegoS32 index;
	for (index = 0; index < m_controllerCount; index++) {
		InputController* controller = m_controllers[index];
		if (p_mode == 1 || controller->m_liveInput == 1) {
			controller->m_recordInput = 0;
		}
		else {
			controller->m_recordInput = 1;
		}
		// Retail calls an empty no-argument helper at 0x00479030 here.
		controller->m_state = 1;
	}
	if (p_mode == 0) {
		m_currentFrame = -1;
		for (index = 0; index < m_controllerCount; index++) {
			m_controllers[index]->m_recordedFrame = -1;
		}
	}
	m_vtable->m_release(this, NULL, mutex);
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004b7d40
void InputControllerManager::Shutdown()
{
	m_vtable->m_shutdown(this, NULL);
	for (LegoS32 index = m_controllerCount - 1; index >= 0; index--) {
		InputController* controller = m_controllers[index];
		if (controller != NULL) {
			controller->~InputController();
			FreeHeapMemory(controller);
		}
		m_controllers[index] = NULL;
	}
}

// STUB: LEGORACERS2 0x004b7dd0
InputDeviceRecord* __fastcall GetControllerInputDevice(InputController* p_controller, void*)
{
	if (p_controller->m_bindingCount == 0) {
		return NULL;
	}
	ControllerBinding* binding = p_controller->GetBinding(0);
	if (p_controller->m_unk0x168 != 0) {
		return (InputDeviceRecord*) p_controller->m_unk0x168;
	}
	switch (binding->m_deviceType) {
	case 1:
		return g_inputControllerManager->m_keyboard;
	case 2:
		return g_inputControllerManager->m_mouse;
	case 3:
		return g_inputService->m_vtable->findDevice(g_inputService, NULL, binding->m_deviceIndex, 3);
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b7f60
InputController* InputControllerManager::FindOrCreateController(
	LegoU32 p_ownerKey,
	LegoU32 p_index,
	LegoBool32 p_create,
	LegoBool32 p_record
)
{
	LegoS32 index;
	for (index = 0; index < m_controllerCount; index++) {
		InputController* controller = m_controllers[index];
		if (controller->m_ownerKey == p_ownerKey && controller->m_index == p_index) {
			return controller;
		}
	}
	if (p_create == 0) {
		return NULL;
	}
	if (index > 11) {
		return m_controllers[0];
	}
	void* memory = AllocateHeapMemory(sizeof(InputController));
	InputController* controller = memory == NULL ? NULL : new (memory) InputController(p_record);
	m_controllers[m_controllerCount] = controller;
	controller->SetOwner(p_ownerKey, p_index, 0);
	controller = m_controllers[m_controllerCount];
	m_controllerCount++;
	return controller;
}

// STUB: LEGORACERS2 0x004b8460
void InputControllerManager::GetTimingState(ControllerTimingState* p_state)
{
	SystemClock* clock = GetSystemClock();
	clock->m_vtable
		->m_queryTime(clock, NULL, (LegoS32*) &p_state->m_absoluteSeconds, (LegoS32*) &p_state->m_absoluteMicroseconds);
	p_state->m_absoluteFrame =
		(p_state->m_absoluteMicroseconds * m_frequency) / 1000000 + p_state->m_absoluteSeconds * m_frequency;
	if (m_paused != 1) {
		p_state->m_relativeSeconds = p_state->m_absoluteSeconds;
		p_state->m_relativeMicroseconds = p_state->m_absoluteMicroseconds;
		p_state->m_relativeMicroseconds -= m_originMicroseconds;
		while ((LegoS32) p_state->m_relativeMicroseconds < 0) {
			p_state->m_relativeMicroseconds += 1000000;
			p_state->m_relativeSeconds--;
		}
		p_state->m_relativeSeconds -= m_originSeconds;
		p_state->m_relativeFrame = p_state->m_absoluteFrame;
		p_state->m_relativeFrame -= m_originFrame;
	}
	else {
		p_state->m_relativeSeconds = m_pausedSeconds;
		p_state->m_relativeMicroseconds = m_pausedMicroseconds;
		p_state->m_relativeFrame = m_pausedFrame;
	}
}

// STUB: LEGORACERS2 0x004b8520
void InputControllerManager::GetElapsedTime(LegoU32* p_seconds, LegoU32* p_microseconds, LegoU32* p_frame)
{
	ControllerTimingState timing = {0};
	GetTimingState(&timing);
	*p_seconds = timing.m_relativeSeconds;
	*p_microseconds = timing.m_relativeMicroseconds;
	*p_frame = timing.m_relativeFrame;
}

// FUNCTION: LEGORACERS2 0x004b8570
LegoU32 InputControllerManager::GetCurrentFrame()
{
	LegoU32 seconds;
	LegoU32 microseconds;
	LegoU32 frame;
	GetElapsedTime(&seconds, &microseconds, &frame);
	return frame / m_pollMultiplier + 1;
}

// STUB: LEGORACERS2 0x004b85a0
LegoBool32 InputControllerManager::PollInput(LegoBool32 p_force)
{
	HANDLE mutex = m_vtable->m_acquire(this, NULL);
	if (mutex == NULL) {
		return FALSE;
	}
	LegoU32 currentFrame = GetCurrentFrame();
	LegoBool32 changed;
	LegoU32 firstFrame;
	LegoU32 frames;
	if ((LegoU32) m_currentFrame < currentFrame || m_currentFrame == -1) {
		changed = TRUE;
		firstFrame = m_currentFrame == -1 ? 0 : m_currentFrame + 1;
		frames = currentFrame - firstFrame + 1;
	}
	else {
		changed = FALSE;
		frames = 0;
	}
	m_currentFrame = currentFrame;
	LegoS32 index;
	if (changed != FALSE || p_force != FALSE) {
		m_keyboard = g_inputService->m_vtable->findDevice(g_inputService, NULL, 0, 1);
		m_mouse = g_inputService->m_vtable->findDevice(g_inputService, NULL, 0, 2);
		g_inputService->m_vtable->refresh(g_inputService, NULL);
		for (index = 0; index < m_controllerCount; index++) {
			InputController* controller = m_controllers[index];
			if (controller->m_disabled == 0) {
				controller->m_unk0x12 = 0;
				controller->m_unk0x10 = 0;
				controller->m_unk0x11 = 0;
				PollController(controller, frames);
			}
		}
	}
	if (changed == 1) {
		for (index = 0; index < m_controllerCount; index++) {
			InputController* controller = m_controllers[index];
			if (controller->m_recordInput == 1 && controller->m_liveInput == 0 && controller->m_disabled == 0) {
				controller->RecordInputRange((LegoU32*) &controller->m_unk0x10, firstFrame, currentFrame);
			}
		}
	}
	m_vtable->m_release(this, NULL, mutex);
	return changed;
}

// STUB: LEGORACERS2 0x004b93e0
static void InitializeInputControllerManagerStorage()
{
	InputControllerManager& manager = g_inputControllerManagerStorage;
	manager.m_currentFrame = -1;
	manager.m_frequency = 0;
	manager.m_pollMultiplier = 0;
	manager.m_controllerCount = 0;
	manager.m_unk0x14 = 0;
	manager.m_unk0x18 = 0;
	manager.m_axisScale0 = 0.1f;
	manager.m_axisScale1 = 0.1f;
	manager.m_unk0x28 = 2;
	manager.m_keyboard = NULL;
	manager.m_mouse = NULL;
	manager.m_paused = 0;
	manager.m_unk0x68 = 0;
	manager.m_unk0x6c = 0;
	manager.m_unk0x70 = 0;
	manager.m_pausedSeconds = 0;
	manager.m_pausedMicroseconds = 0;
	manager.m_unk0x7c = 0;
	manager.m_pausedFrame = 0;
	manager.m_originSeconds = 0;
	manager.m_originMicroseconds = 0;
	manager.m_unk0x8c = 0;
	manager.m_unk0x90 = 0;
	manager.m_originFrame = 0;
	LegoU32 index;
	for (index = 0; index < sizeOfArray(manager.m_unk0x98); index++) {
		manager.m_unk0x98[index] = 0;
	}
	for (index = 0; index < sizeOfArray(manager.m_controllers); index++) {
		manager.m_controllers[index] = NULL;
	}
	manager.m_vtable = &g_controllerManagerDispatch;
}

// Retail initializes this fixed BSS object through its CRT initializer list.
static int g_inputControllerManagerInitialized = (InitializeInputControllerManagerStorage(), 0);
