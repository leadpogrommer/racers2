#ifndef LR2_SYSTEMCLOCK_H
#define LR2_SYSTEMCLOCK_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>
#include <windows.h>

typedef unsigned __int64 SystemClockU64;
typedef __int64 SystemClockS64;

class SystemClock;

// The original table is an eight-entry dispatch table. The explicit ECX/EDX
// bridge types preserve its x86 callback ABI without embedding retail code
// addresses in the rebuilt image.
struct SystemClockDispatch {
	typedef void(__fastcall* Action)(SystemClock*, void*);
	typedef SystemClockS64(__fastcall* Value64)(SystemClock*, void*);
	typedef SystemClockU64(__fastcall* UnsignedValue64)(SystemClock*, void*);
	typedef DWORD(WINAPI* ReadMilliseconds)();
	typedef void(__fastcall* QueryTime)(SystemClock*, void*, LegoS32*, LegoS32*);
	typedef LegoU32(__fastcall* Value)(SystemClock*, void*);

	Action m_pushCycles;
	UnsignedValue64 m_popCycles;
	ReadMilliseconds m_readMilliseconds;
	QueryTime m_queryTime;
	Value64 m_updateElapsedTime;
	Action m_reset;
	UnsignedValue64 m_zeroValue;
	Value m_getCalibration;
};

class SystemClock {
public:
	void Initialize();

	// These members are the object projection evidenced by the constructor,
	// reset/query methods, and the singleton's adjacent initialization guard.
	const SystemClockDispatch* m_vtable;
	LegoS32 m_stackDepth;
	SystemClockU64 m_cycleStack[10];
	SystemClockU64 m_cyclesPerSecond;
	SystemClockU64 m_queryStartCycles;
	SystemClockU64 m_lastCycles;
	LegoU32 m_calibrationRate;
};

SystemClock* GetSystemClock();
SystemClockU64 ReadSystemClockCycles();
extern const SystemClockDispatch g_systemClockDispatch;

DECOMP_SIZE_ASSERT(SystemClockDispatch, 0x20);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_vtable) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_stackDepth) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_cycleStack) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_cyclesPerSecond) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_queryStartCycles) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_lastCycles) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(SystemClock, m_calibrationRate) == 0x70);

#endif // LR2_SYSTEMCLOCK_H
