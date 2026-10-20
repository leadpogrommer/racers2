#include "input/controller/inputcontrollermanager.h"

static void CALLBACK ControllerPollTimerCallback(UINT, UINT, DWORD, DWORD, DWORD);

LegoBool32 __fastcall InitializeControllerPolling(InputControllerManager* p_manager, void*)
{
	return p_manager->InitializePolling();
}

void __fastcall StopControllerPolling(InputControllerManager* p_manager, void*)
{
	p_manager->StopPolling();
}

HANDLE __fastcall AcquireControllerPollMutex(InputControllerManager* p_manager, void*)
{
	return p_manager->AcquirePollMutex();
}

void __fastcall ReleaseControllerPollMutex(InputControllerManager* p_manager, void*, HANDLE p_mutex)
{
	p_manager->ReleasePollMutex(p_mutex);
}

const ControllerManagerDispatch g_controllerManagerDispatch =
	{InitializeControllerPolling, StopControllerPolling, AcquireControllerPollMutex, ReleaseControllerPollMutex};

// STUB: LEGORACERS2 0x004b94c0
HANDLE InputControllerManager::AcquirePollMutex()
{
	return g_multimediaTimerService->AcquireNamedMutex("ATDControlInterfacePollMutex", FALSE);
}

// FUNCTION: LEGORACERS2 0x004b94e0
void InputControllerManager::ReleasePollMutex(HANDLE p_mutex)
{
	g_multimediaTimerService->ReleaseNamedMutex(&p_mutex);
}

// FUNCTION: LEGORACERS2 0x004b9500
LegoBool32 InputControllerManager::InitializePolling()
{
	InitializeMultimediaTimerService();
	g_multimediaTimerService->CreateNamedMutex("ATDControlInterfacePollMutex", &m_pollMutex);
	g_multimediaTimerService
		->CreatePeriodicEvent(&m_timerId, ControllerPollTimerCallback, (m_pollMultiplier * 1000) / m_frequency - 5);
	PollInput(TRUE);
	return TRUE;
}

// STUB: LEGORACERS2 0x004b9560
void InputControllerManager::StopPolling()
{
	HANDLE mutex = m_vtable->m_acquire(this, NULL);
	while (mutex == NULL) {
		mutex = m_vtable->m_acquire(this, NULL);
	}
	g_multimediaTimerService->KillPeriodicEvent(&m_timerId);
	m_vtable->m_release(this, NULL, mutex);
	g_multimediaTimerService->CloseMutex(&m_pollMutex);
	ShutdownMultimediaTimerService();
}

// FUNCTION: LEGORACERS2 0x004b95b0
static void CALLBACK ControllerPollTimerCallback(UINT, UINT, DWORD, DWORD, DWORD)
{
	g_inputControllerManagerStorage.PollInput(TRUE);
}
