#ifndef LR2_CONTROLLEREVENTRECORDS_H
#define LR2_CONTROLLEREVENTRECORDS_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct ControllerEventRecord {
	LegoS32 m_index;
	LegoFloat m_u;
	LegoFloat m_v;
	LegoFloat m_uEnd;
	LegoFloat m_vEnd;
};
DECOMP_SIZE_ASSERT(ControllerEventRecord, 0x14);

// The first word is also tested as resource flags by the HUD texture loader.
// Other consumers of this large record collection remain under investigation.
struct ControllerEventRecordTable {
	LegoU32 m_flags;
	ControllerEventRecord m_records[280];
	LegoS32 m_firstIndex;
	LegoS32 m_secondIndex;
	void SetPixelTextureRegion(LegoS32, LegoS32, LegoS32, LegoS32, LegoU32, LegoFloat, LegoFloat);
	void SetFullTextureRegion(LegoU32);
};
DECOMP_STATIC_ASSERT(offsetof(ControllerEventRecordTable, m_firstIndex) == 0x15e4);
DECOMP_SIZE_ASSERT(ControllerEventRecordTable, 0x15ec);

void __fastcall LoadControllerHudTextures(void*, void*);
void __fastcall ReleaseControllerHudTextures(void*, void*);

#endif
