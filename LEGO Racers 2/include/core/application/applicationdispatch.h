#ifndef LR2_APPLICATIONDISPATCH_H
#define LR2_APPLICATIONDISPATCH_H

#include "decomp.h"
#include "types.h"

#include <windows.h>

class Application;
struct ApplicationDisplaySettings;

// Explicit game ABI, not a proposed source inheritance hierarchy. The first
// fastcall argument occupies retail's ECX receiver. The unused EDX argument
// leaves every explicit argument on the stack with retail's callee cleanup.
struct ApplicationDispatch {
	typedef void(__fastcall* Action)(Application*, void*);
	typedef LegoU32(__fastcall* Value)(Application*, void*);
	typedef LegoFloat(__fastcall* FloatValue)(Application*, void*);
	typedef Application*(__fastcall* DeleteObject)(Application*, void*, LegoU32);
	typedef LegoBool32(__fastcall* MessagePolicy)(Application*, void*, MSG*);
	typedef const LegoChar*(__fastcall* Title)(Application*, void*);
	typedef LegoBool32(__fastcall* WindowMessage)(Application*, void*, HWND, UINT, WPARAM, LPARAM);
	typedef ApplicationDisplaySettings*(__fastcall* DisplaySettings)(Application*, void*);
	typedef void(__fastcall* SetDisplaySettings)(Application*, void*, const ApplicationDisplaySettings*, LegoBool32);
	typedef void(__fastcall* Diagnostic)(Application*, void*, const LegoChar*);
	typedef void(__fastcall* TwoArguments)(Application*, void*, LegoU32, LegoU32);

	Action m_loadConfig;                     // 0x00
	Action m_saveConfig;                     // 0x04
	DeleteObject m_delete;                   // 0x08
	FloatValue m_float0x0c;                  // 0x0c
	Action m_action0x10;                     // 0x10
	Value m_developmentMode;                 // 0x14
	Value m_updateAfterMessage;              // 0x18
	Value m_value0x1c;                       // 0x1c
	MessagePolicy m_dispatchMessage;         // 0x20
	Value m_pollMessages;                    // 0x24
	Action m_action0x28;                     // 0x28
	Title m_title;                           // 0x2c
	Value m_value0x30;                       // 0x30
	Value m_unknownFlag;                     // 0x34
	WindowMessage m_windowMessage;           // 0x38
	Value m_iconResource;                    // 0x3c
	Value m_acceleratorResource;             // 0x40
	Action m_beforeStartup;                  // 0x44
	Action m_afterStartup;                   // 0x48
	Value m_heapReserve;                     // 0x4c
	Value m_value0x50;                       // 0x50
	Value m_value0x54;                       // 0x54
	Action m_action0x58;                     // 0x58
	Value m_value0x5c;                       // 0x5c
	Value m_value0x60;                       // 0x60
	Value m_value0x64;                       // 0x64
	Value m_value0x68;                       // 0x68
	Value m_value0x6c;                       // 0x6c
	Value m_frameRate;                       // 0x70
	Value m_value0x74;                       // 0x74
	FloatValue m_updateInterval;             // 0x78
	Value m_value0x7c;                       // 0x7c
	DisplaySettings m_displaySettings;       // 0x80
	SetDisplaySettings m_setDisplaySettings; // 0x84
	Diagnostic m_diagnostic;                 // 0x88
	TwoArguments m_action0x8c;               // 0x8c
	Action m_settingsDialog;                 // 0x90, exit-state target has no defined result
	Value m_value0x94;                       // 0x94
};

DECOMP_SIZE_ASSERT(ApplicationDispatch, 0x98);

extern const ApplicationDispatch g_applicationPrimaryDispatch;
extern const ApplicationDispatch g_applicationExitDispatch;

#endif // LR2_APPLICATIONDISPATCH_H
