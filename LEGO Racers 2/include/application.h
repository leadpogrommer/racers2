#ifndef LR2_APPLICATION_H
#define LR2_APPLICATION_H

#include "applicationdispatch.h"
#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct ApplicationDisplaySettings {
	LegoS32 m_width;
	LegoS32 m_height;
	LegoS32 m_bitDepth;
};

DECOMP_SIZE_ASSERT(ApplicationDisplaySettings, 0x0c);

// Observed state prefix. Its complete original allocation size/source class
// hierarchy remains unknown; the explicit dispatch ABI has all 38 slots.
class Application {
public:
	Application();
	ApplicationDisplaySettings* GetDisplaySettings();
	LegoU32 GetUnknownFlag();
	void LoadDisplayConfig();
	void SaveDisplayConfig();
	LegoBool32 IsDevelopmentMode() { return m_vtable->m_developmentMode(this, NULL); }
	LegoU32 GetHeapReserveSize() { return m_vtable->m_heapReserve(this, NULL); }
	LegoU32 GetIconResourceId() { return m_vtable->m_iconResource(this, NULL); }
	LegoU32 GetAcceleratorResourceId() { return m_vtable->m_acceleratorResource(this, NULL); }
	void NotifyDiagnostic(const LegoChar* p_message) { m_vtable->m_diagnostic(this, NULL, p_message); }

	const ApplicationDispatch* m_vtable;  // 0x00
	ApplicationDisplaySettings m_display; // 0x04
	LegoU32 m_unk0x10;                    // 0x10
};

DECOMP_STATIC_ASSERT(offsetof(Application, m_display) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(Application, m_unk0x10) == 0x10);

Application* GetApplication();

#endif // LR2_APPLICATION_H
