#include "platform/timing/systemclock.h"

#include <mmsystem.h>
#include <stdlib.h>

static void __fastcall PushCycleCounter(SystemClock* p_clock, void* p_edx);
static SystemClockU64 __fastcall PopCycleCounter(SystemClock* p_clock, void* p_edx);
static void __fastcall QueryClockTime(SystemClock* p_clock, void* p_edx, LegoS32* p_seconds, LegoS32* p_microseconds);
static SystemClockS64 __fastcall UpdateElapsedTime(SystemClock* p_clock, void* p_edx);
static void __fastcall ResetClock(SystemClock* p_clock, void* p_edx);
static SystemClockU64 __fastcall ReturnZeroClockValue(SystemClock* p_clock, void* p_edx);
static LegoU32 __fastcall GetClockCalibration(SystemClock* p_clock, void* p_edx);
static void __cdecl SystemClockProcessExit();

// Keep the dispatch table statically initialized under VC6. An imported
// function address in its initializer otherwise produces CRT-time writes.
static DWORD WINAPI ReadClockMilliseconds()
{
	return timeGetTime();
}

const SystemClockDispatch g_systemClockDispatch = {
	PushCycleCounter,
	PopCycleCounter,
	ReadClockMilliseconds,
	QueryClockTime,
	UpdateElapsedTime,
	ResetClock,
	ReturnZeroClockValue,
	GetClockCalibration
};

// GLOBAL: LEGORACERS2 0x00654ba0
static SystemClock g_systemClockStorage;

// GLOBAL: LEGORACERS2 0x00654c18
static LegoU8 g_systemClockInitialized;

// The retail routine preserves the general-purpose registers around RDTSC.
// Keep this as the genuine instruction rather than substituting a wall clock.
// STUB: LEGORACERS2 0x004b2960
SystemClock* GetSystemClock()
{
	if ((g_systemClockInitialized & 1) == 0) {
		g_systemClockInitialized |= 1;
		g_systemClockStorage.Initialize();
		atexit(SystemClockProcessExit);
	}
	return &g_systemClockStorage;
}

// Retail exit callback at 0x004b29a0 is a single `ret`.
static void __cdecl SystemClockProcessExit()
{
}

// STUB: LEGORACERS2 0x004b29b0
void SystemClock::Initialize()
{
	m_vtable = &g_systemClockDispatch;
	m_stackDepth = 0;

	HANDLE process = GetCurrentProcess();
	SetPriorityClass(process, HIGH_PRIORITY_CLASS);

	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency)) {
		DWORD start = timeGetTime();
		DWORD now;
		do {
			now = timeGetTime();
		} while (now == start);

		PushCycleCounter(this, NULL);
		DWORD deadline = now + 250;
		while (timeGetTime() < deadline) {
		}
	}
	else {
		LARGE_INTEGER initial;
		LARGE_INTEGER current;
		QueryPerformanceCounter(&initial);
		do {
			QueryPerformanceCounter(&current);
		} while (current.QuadPart == initial.QuadPart);

		PushCycleCounter(this, NULL);
		LARGE_INTEGER deadline;
		deadline.QuadPart = current.QuadPart + frequency.QuadPart / 4;
		do {
			QueryPerformanceCounter(&current);
		} while (current.QuadPart < deadline.QuadPart);
	}

	m_cyclesPerSecond = PopCycleCounter(this, NULL) * 4;

	volatile LegoFloat benchmarkResult = 0.0f;
	volatile LegoFloat one = 1.0f;
	volatile LegoFloat two = 2.0f;
	volatile LegoFloat three = 3.0f;
	volatile LegoFloat four = 4.0f;
	PushCycleCounter(this, NULL);
	for (LegoS32 i = 0; i < 100000; ++i) {
		benchmarkResult = one * two + three - four;
	}
	SystemClockU64 benchmarkCycles = PopCycleCounter(this, NULL);
	if ((SystemClockS64) benchmarkCycles <= 1000) {
		m_calibrationRate = 10000;
	}
	else {
		m_calibrationRate = (LegoU32) (10000000 / (SystemClockS64) benchmarkCycles);
	}

	SetPriorityClass(process, NORMAL_PRIORITY_CLASS);
	ResetClock(this, NULL);
}

// FUNCTION: LEGORACERS2 0x004b2b50
static SystemClockU64 __fastcall ReturnZeroClockValue(SystemClock* p_clock, void* p_edx)
{
	(void) p_clock;
	(void) p_edx;
	return 0;
}

// FUNCTION: LEGORACERS2 0x004b2b60
static LegoU32 __fastcall GetClockCalibration(SystemClock* p_clock, void* p_edx)
{
	(void) p_edx;
	return p_clock->m_calibrationRate;
}

// STUB: LEGORACERS2 0x004b2b70
static void __fastcall ResetClock(SystemClock* p_clock, void* p_edx)
{
	(void) p_edx;
	SystemClockU64 cycles = ReadSystemClockCycles();
	p_clock->m_lastCycles = cycles;
	p_clock->m_queryStartCycles = cycles;
}

// STUB: LEGORACERS2 0x004b2b90
static void __fastcall PushCycleCounter(SystemClock* p_clock, void* p_edx)
{
	(void) p_edx;
	SystemClockU64 cycles = ReadSystemClockCycles();
	LegoS32 index = p_clock->m_stackDepth;
	p_clock->m_stackDepth = index + 1;
	p_clock->m_cycleStack[index] = cycles;
}

// STUB: LEGORACERS2 0x004b2bc0
static SystemClockU64 __fastcall PopCycleCounter(SystemClock* p_clock, void* p_edx)
{
	(void) p_edx;
	SystemClockU64 cycles = ReadSystemClockCycles();
	p_clock->m_stackDepth -= 1;
	return (SystemClockS64) (cycles - p_clock->m_cycleStack[p_clock->m_stackDepth]);
}

// STUB: LEGORACERS2 0x004b2c00
SystemClockU64 ReadSystemClockCycles()
{
	LegoU32 cyclesLow;
	LegoU32 cyclesHigh;
	__asm {
		push eax
		push edx
		push ecx
		mov ecx, 10h
		rdtsc
		pop ecx
		mov cyclesLow, eax
		mov cyclesHigh, edx
		pop edx
		pop eax
	}
	return ((SystemClockU64) cyclesHigh << 32) | cyclesLow;
}

// STUB: LEGORACERS2 0x004b2c40
static void __fastcall QueryClockTime(SystemClock* p_clock, void* p_edx, LegoS32* p_seconds, LegoS32* p_microseconds)
{
	(void) p_edx;
	SystemClockS64 elapsed = (SystemClockS64) ReadSystemClockCycles() - (SystemClockS64) p_clock->m_queryStartCycles;
	SystemClockS64 rate = (SystemClockS64) p_clock->m_cyclesPerSecond;
	SystemClockS64 seconds = elapsed / rate;
	SystemClockS64 remainder = elapsed - seconds * rate;
	LegoS32 microseconds = (LegoS32) ((remainder * 1000000) / rate);
	*p_seconds = (LegoS32) seconds;
	*p_microseconds = microseconds;
}

// FUNCTION: LEGORACERS2 0x004b2cc0
static SystemClockS64 __fastcall UpdateElapsedTime(SystemClock* p_clock, void* p_edx)
{
	(void) p_edx;
	SystemClockU64 cycles = ReadSystemClockCycles();
	SystemClockS64 elapsed = (SystemClockS64) cycles - (SystemClockS64) p_clock->m_lastCycles;
	SystemClockS64 elapsedMicroseconds = (elapsed * 1000000) / (SystemClockS64) p_clock->m_cyclesPerSecond;
	p_clock->m_lastCycles = cycles;
	return elapsedMicroseconds;
}
