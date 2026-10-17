#ifndef LR2_MULTIMEDIATIMER_H
#define LR2_MULTIMEDIATIMER_H

#include "types.h"

#include <windows.h>
// The SDK multimedia declarations require the Win32 types above.
#include <mmsystem.h>

// Observed WinMM/mutex adapter used by the controller polling service.
// Its library provenance is unresolved; these are descriptive working names.
struct MultimediaTimerService {
	void EndPeriod();
	LegoBool32 CreatePeriodicEvent(MMRESULT* p_event, LPTIMECALLBACK p_callback, UINT p_delay);
	void KillPeriodicEvent(MMRESULT* p_event);
	LegoBool32 CreateNamedMutex(const LegoChar* p_name, HANDLE* p_mutex);
	void CloseMutex(HANDLE* p_mutex);
	HANDLE AcquireNamedMutex(const LegoChar* p_name, LegoBool32 p_wait);
	void ReleaseNamedMutex(HANDLE* p_mutex);

	UINT m_period;
};

extern MultimediaTimerService* g_multimediaTimerService;
LegoBool32 InitializeMultimediaTimerService();
void ShutdownMultimediaTimerService();

#endif // LR2_MULTIMEDIATIMER_H
