#include "multimediatimer.h"

// GLOBAL: LEGORACERS2 0x006e4e64
static MultimediaTimerService g_multimediaTimerStorage;

// GLOBAL: LEGORACERS2 0x006e4e6c
MultimediaTimerService* g_multimediaTimerService = NULL;

// STUB: LEGORACERS2 0x004f4a40
LegoBool32 InitializeMultimediaTimerService()
{
	if (g_multimediaTimerService != NULL) {
		return TRUE;
	}
	g_multimediaTimerService = &g_multimediaTimerStorage;
	TIMECAPS capabilities;
	if (timeGetDevCaps(&capabilities, sizeof(capabilities)) != TIMERR_NOERROR) {
		return FALSE;
	}
	UINT period = capabilities.wPeriodMin;
	if (period <= 5) {
		period = 5;
	}
	if (period >= capabilities.wPeriodMax) {
		period = capabilities.wPeriodMax;
	}
	g_multimediaTimerStorage.m_period = period;
	if (timeBeginPeriod(period) != TIMERR_NOERROR) {
		g_multimediaTimerStorage.m_period = 0;
		return FALSE;
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004f4ad0
void ShutdownMultimediaTimerService()
{
	if (g_multimediaTimerService != NULL) {
		g_multimediaTimerService->EndPeriod();
		g_multimediaTimerService = NULL;
	}
}

// FUNCTION: LEGORACERS2 0x004f4af0
void MultimediaTimerService::EndPeriod()
{
	if (m_period != 0) {
		timeEndPeriod(m_period);
		m_period = 0;
	}
}

// FUNCTION: LEGORACERS2 0x004f4b10
LegoBool32 MultimediaTimerService::CreatePeriodicEvent(MMRESULT* p_event, LPTIMECALLBACK p_callback, UINT p_delay)
{
	MMRESULT event = timeSetEvent(p_delay, m_period, p_callback, 0, TIME_PERIODIC);
	*p_event = event;
	return event != 0;
}

// FUNCTION: LEGORACERS2 0x004f4b40
void MultimediaTimerService::KillPeriodicEvent(MMRESULT* p_event)
{
	if (*p_event != 0) {
		timeKillEvent(*p_event);
		*p_event = 0;
	}
}

// FUNCTION: LEGORACERS2 0x004f4b60
LegoBool32 MultimediaTimerService::CreateNamedMutex(const LegoChar* p_name, HANDLE* p_mutex)
{
	HANDLE mutex = CreateMutex(NULL, FALSE, p_name);
	*p_mutex = mutex;
	return mutex != NULL;
}

// FUNCTION: LEGORACERS2 0x004f4b90
void MultimediaTimerService::CloseMutex(HANDLE* p_mutex)
{
	if (*p_mutex != NULL) {
		CloseHandle(*p_mutex);
	}
	*p_mutex = NULL;
}

// STUB: LEGORACERS2 0x004f4bb0
HANDLE MultimediaTimerService::AcquireNamedMutex(const LegoChar* p_name, LegoBool32 p_wait)
{
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, p_name);
	if (mutex != NULL) {
		DWORD timeout = p_wait != FALSE ? INFINITE : 0;
		if (WaitForSingleObject(mutex, timeout) != WAIT_OBJECT_0) {
			ReleaseMutex(mutex);
			CloseHandle(mutex);
			return NULL;
		}
	}
	return mutex;
}

// FUNCTION: LEGORACERS2 0x004f4c00
void MultimediaTimerService::ReleaseNamedMutex(HANDLE* p_mutex)
{
	if (*p_mutex != NULL) {
		ReleaseMutex(*p_mutex);
		CloseHandle(*p_mutex);
		*p_mutex = NULL;
	}
}
