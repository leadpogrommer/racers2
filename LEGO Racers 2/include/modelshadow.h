#ifndef LR2_MODELSHADOW_H
#define LR2_MODELSHADOW_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

class WinFile;
struct ModelResource;

struct ModelShadowData {
	void* m_edgeRecords;             // 0x00, 0x24-byte records
	void* m_faceRecords;             // 0x04, 0x0c-byte records
	LegoU32 m_edgeRecordCount;       // 0x08
	LegoU32 m_faceRecordCount;       // 0x0c
	LegoU32 m_unk0x10;               // 0x10
	LegoFloat m_scaleX;              // 0x14
	LegoFloat m_scaleY;              // 0x18
	LegoU32 m_unk0x1c[3];            // 0x1c
	LegoS32 m_textureId;             // 0x28
	LegoU32 m_textureReferenceMask;  // 0x2c
	LegoU32 m_geometryReferenceMask; // 0x30
	LegoU32 m_referenceCount;        // 0x34
};
DECOMP_SIZE_ASSERT(ModelShadowData, 0x38);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_edgeRecords) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_faceRecords) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_edgeRecordCount) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_faceRecordCount) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_textureId) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_textureReferenceMask) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_geometryReferenceMask) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(ModelShadowData, m_referenceCount) == 0x34);

void __fastcall ReleaseModelShadowData(ModelResource* p_model, void* p_unusedEdx);
void __fastcall ConfigureModelShadow(ModelResource*, void*, LegoU32, const LegoChar*, LegoU32);
void __fastcall GenerateModelShadowData(ModelResource*, void*, LegoU32, const LegoChar*);

#endif // LR2_MODELSHADOW_H
