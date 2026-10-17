#include "application.h"
#include "applicationdialog.h"
#include "applicationmessages.h"
#include "applicationpolicy.h"
#include "memoryheap.h"

#include <new.h>
#include <stdlib.h>

// These bridges preserve the observed ECX/stack ABI without guessing the
// original C++ inheritance hierarchy. EDX is deliberately unused.
void __fastcall InvokeApplicationLoadConfig(Application* p_app, void*)
{
	p_app->LoadDisplayConfig();
}
void __fastcall InvokeApplicationSaveConfig(Application* p_app, void*)
{
	p_app->SaveDisplayConfig();
}
LegoU32 __fastcall InvokeApplicationUnknownFlag(Application* p_app, void*)
{
	return p_app->GetUnknownFlag();
}
ApplicationDisplaySettings* __fastcall InvokeApplicationDisplaySettings(Application* p_app, void*)
{
	return p_app->GetDisplaySettings();
}

// Retail 0x0049a6b0: scalar deleting destructor, flags bit 0 controls freeing.
Application* __fastcall DeleteApplicationObject(Application* p_app, void*, LegoU32 p_flags)
{
	p_app->m_vtable = &g_applicationExitDispatch;
	if (p_flags & 1) {
		FreeHeapMemory(p_app);
	}
	return p_app;
}

// The no-op targets do not define a result in EAX; do not invent one.
void ApplicationNoOp()
{
}
void WINAPI ApplicationTwoArgumentNoOp(LegoU32, LegoU32)
{
}
LegoU32 ApplicationTruePolicy()
{
	return TRUE;
}
LegoFloat ApplicationPrimaryFloat()
{
	return 0.901f;
}
LegoFloat ApplicationZeroFloat()
{
	return 0.0f;
}
LegoBool32 WINAPI AcceptApplicationMessage(MSG*)
{
	return TRUE;
}
LegoBool32 WINAPI AcceptApplicationWindowMessage(HWND, UINT, WPARAM, LPARAM)
{
	return TRUE;
}
const LegoChar* ApplicationGameTitle()
{
	return "LEGO Racers 2";
}
const LegoChar* ApplicationBaseTitle()
{
	return "NextGen Game";
}
LegoU32 ApplicationBaseHeapReserve()
{
	return 0x01c00000;
}
LegoU32 ApplicationPrimaryValue50()
{
	return 0x6d60;
}
LegoU32 ApplicationBaseValue50()
{
	return 0x2000;
}
LegoU32 ApplicationValue5c()
{
	return 0xffff;
}
LegoU32 ApplicationValue60()
{
	return 0x8000;
}
LegoU32 ApplicationPrimaryValue64()
{
	return 1500;
}
LegoU32 ApplicationBaseValue64()
{
	return 1024;
}
LegoU32 ApplicationPrimaryFrameRate()
{
	return 30;
}
LegoU32 ApplicationBaseFrameRate()
{
	return 60;
}
LegoU32 ApplicationBaseValue74()
{
	return 2;
}
LegoFloat ApplicationUpdateInterval()
{
	Application* app = GetApplication();
	return 1.0f / (LegoS32) app->m_vtable->m_frameRate(app, NULL);
}
void __fastcall SetApplicationDisplaySettings(
	Application* p_app,
	void*,
	const ApplicationDisplaySettings* p_settings,
	LegoBool32 p_save
)
{
	p_app->m_display = *p_settings;
	if (p_save) {
		p_app->m_vtable->m_saveConfig(p_app, NULL);
	}
}
void __fastcall SetApplicationDisplaySettingsWithoutSave(
	Application* p_app,
	void*,
	const ApplicationDisplaySettings* p_settings,
	LegoBool32
)
{
	p_app->m_display = *p_settings;
}
INT_PTR InvokeApplicationSettingsDialog()
{
	return ShowApplicationSettingsDialog();
}

// Retail tables 0x00534994 and 0x00534a2c. The wiring verifier checks every
// original target and every emitted pointer, including genuine folded aliases.
// ABI bridges are measured separately from the original method bodies.
const ApplicationDispatch g_applicationPrimaryDispatch = {
	reinterpret_cast<ApplicationDispatch::Action>(InvokeApplicationLoadConfig),               // 0x00
	reinterpret_cast<ApplicationDispatch::Action>(InvokeApplicationSaveConfig),               // 0x04
	reinterpret_cast<ApplicationDispatch::DeleteObject>(DeleteApplicationObject),             // 0x08
	reinterpret_cast<ApplicationDispatch::FloatValue>(ApplicationPrimaryFloat),               // 0x0c
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                           // 0x10
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x14
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x18
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x1c
	reinterpret_cast<ApplicationDispatch::MessagePolicy>(AcceptApplicationMessage),           // 0x20
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationTruePolicy),                      // 0x24
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                           // 0x28
	reinterpret_cast<ApplicationDispatch::Title>(ApplicationGameTitle),                       // 0x2c
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x30
	reinterpret_cast<ApplicationDispatch::Value>(InvokeApplicationUnknownFlag),               // 0x34
	reinterpret_cast<ApplicationDispatch::WindowMessage>(FilterApplicationWindowMessage),     // 0x38
	reinterpret_cast<ApplicationDispatch::Value>(GetApplicationIconResourceId),               // 0x3c
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x40
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                           // 0x44
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                           // 0x48
	reinterpret_cast<ApplicationDispatch::Value>(GetApplicationHeapReserve),                  // 0x4c
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationPrimaryValue50),                  // 0x50
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationTruePolicy),                      // 0x54
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                           // 0x58
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationValue5c),                         // 0x5c
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationValue60),                         // 0x60
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationPrimaryValue64),                  // 0x64
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x68
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x6c
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationPrimaryFrameRate),                // 0x70
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationTruePolicy),                      // 0x74
	reinterpret_cast<ApplicationDispatch::FloatValue>(ApplicationUpdateInterval),             // 0x78
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x7c
	reinterpret_cast<ApplicationDispatch::DisplaySettings>(InvokeApplicationDisplaySettings), // 0x80
	reinterpret_cast<ApplicationDispatch::SetDisplaySettings>(SetApplicationDisplaySettings), // 0x84
	reinterpret_cast<ApplicationDispatch::Diagnostic>(OnApplicationDiagnostic),               // 0x88
	reinterpret_cast<ApplicationDispatch::TwoArguments>(ApplicationTwoArgumentNoOp),          // 0x8c
	reinterpret_cast<ApplicationDispatch::Action>(InvokeApplicationSettingsDialog),           // 0x90
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),               // 0x94
};

const ApplicationDispatch g_applicationExitDispatch = {
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x00
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x04
	reinterpret_cast<ApplicationDispatch::DeleteObject>(DeleteApplicationObject),                        // 0x08
	reinterpret_cast<ApplicationDispatch::FloatValue>(ApplicationZeroFloat),                             // 0x0c
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x10
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x14
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x18
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x1c
	reinterpret_cast<ApplicationDispatch::MessagePolicy>(AcceptApplicationMessage),                      // 0x20
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationTruePolicy),                                 // 0x24
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x28
	reinterpret_cast<ApplicationDispatch::Title>(ApplicationBaseTitle),                                  // 0x2c
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x30
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x34
	reinterpret_cast<ApplicationDispatch::WindowMessage>(AcceptApplicationWindowMessage),                // 0x38
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x3c
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x40
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x44
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x48
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationBaseHeapReserve),                            // 0x4c
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationBaseValue50),                                // 0x50
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationTruePolicy),                                 // 0x54
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x58
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationValue5c),                                    // 0x5c
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationValue60),                                    // 0x60
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationBaseValue64),                                // 0x64
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x68
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x6c
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationBaseFrameRate),                              // 0x70
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationBaseValue74),                                // 0x74
	reinterpret_cast<ApplicationDispatch::FloatValue>(ApplicationUpdateInterval),                        // 0x78
	reinterpret_cast<ApplicationDispatch::Value>(IsApplicationDevelopmentMode),                          // 0x7c
	reinterpret_cast<ApplicationDispatch::DisplaySettings>(InvokeApplicationDisplaySettings),            // 0x80
	reinterpret_cast<ApplicationDispatch::SetDisplaySettings>(SetApplicationDisplaySettingsWithoutSave), // 0x84
	reinterpret_cast<ApplicationDispatch::Diagnostic>(OnApplicationDiagnostic),                          // 0x88
	reinterpret_cast<ApplicationDispatch::TwoArguments>(ApplicationTwoArgumentNoOp),                     // 0x8c
	reinterpret_cast<ApplicationDispatch::Action>(ApplicationNoOp),                                      // 0x90
	reinterpret_cast<ApplicationDispatch::Value>(ApplicationTruePolicy),                                 // 0x94
};

// GLOBAL: LEGORACERS2 0x00578774
static LegoU8 g_applicationInitialized = 0;

// Retail 0x00578648: storage for the observed state prefix, not a claim of
// the complete original object's size. No constructor runs before the guard.
// GLOBAL: LEGORACERS2 0x00578648
static LegoU32 g_applicationStorage[sizeof(Application) / sizeof(LegoU32)];

static void ResetApplicationAtExit();

// STUB: LEGORACERS2 0x0049a610
Application::Application()
{
	m_display.m_width = 640;
	m_display.m_height = 480;
	m_display.m_bitDepth = 32;
	m_unk0x10 = 0;
	m_vtable = &g_applicationPrimaryDispatch;
	LoadDisplayConfig();
}

// STUB: LEGORACERS2 0x0049a760
Application* GetApplication()
{
	if ((g_applicationInitialized & 1) == 0) {
		g_applicationInitialized |= 1;
		new (g_applicationStorage) Application;
		atexit(ResetApplicationAtExit);
	}
	return reinterpret_cast<Application*>(g_applicationStorage);
}

// STUB: LEGORACERS2 0x0049a7a0
static void ResetApplicationAtExit()
{
	reinterpret_cast<Application*>(g_applicationStorage)->m_vtable = &g_applicationExitDispatch;
}
