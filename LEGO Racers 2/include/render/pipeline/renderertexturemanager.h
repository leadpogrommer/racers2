#ifndef LR2_RENDERERTEXTUREMANAGER_H
#define LR2_RENDERERTEXTUREMANAGER_H

#include "decomp.h"
#include "directx8.h"
#include "render/pipeline/rendererrecordgroup.h"
#include "render/resources/rendererrecords.h"
#include "types.h"

#include <stddef.h>

struct RendererStagePlanRequest;
struct RendererDefaultRecord;

struct RendererTextureStageRoute {
	LegoU32 m_enabled;
	LegoU32 m_priority;
};

DECOMP_SIZE_ASSERT(RendererTextureStageRoute, 0x08);

struct RendererTextureOperationProfile {
	LegoU32 m_colorOperation;
	LegoU32 m_colorArgument1;
	LegoU32 m_colorArgument2;
	LegoU32 m_alphaOperation;
	LegoU32 m_alphaArgument1;
	LegoU32 m_alphaArgument2;
	LegoU32 m_unk0x18;
};

DECOMP_SIZE_ASSERT(RendererTextureOperationProfile, 0x1c);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureOperationProfile, m_alphaOperation) == 0x0c);

class RendererTextureStageManager {
public:
	void Initialize();
	LegoU32 BuildProfileStagePlan(RendererStagePlanRequest* p_request, LegoU32 p_unk0x04);
	LegoU32 BuildDefaultStagePlan(RendererStagePlanRequest* p_request, LegoU32 p_mode);
	LegoU32 ApplyStagePlanSubrecord(RendererDefaultRecord* p_defaults, LegoS32 p_index);
	LegoBool32 AreTextureOperationsSupported(LegoU32 p_profileIndex) const;
	LegoU32 MaskWithoutHighestPriorityStage(LegoU32 p_stageMask) const;
	RendererSubrecordGroup* FindSubrecordGroup(LegoU32 p_key);
	RendererSubrecordGroup* CreateSubrecordGroup(LegoU32 p_key);
	void CopySubrecordGroupCacheEntry(LegoU32 p_destinationKey, LegoU32 p_sourceKey);

	RendererSubrecordGroup m_subrecordGroups[32]; // 0x0000..0x187f
	RendererArrayMarker m_stageRecords[256];      // 0x1880..0x2c7f
	BYTE m_recordIndexCache[0x800];               // 0x2c80..0x347f, initialized to 0xff
	LegoU32 m_unk0x3480;
	LegoU32 m_unk0x3484;
	LegoU32 m_unk0x3488;
	LegoU32 m_unk0x348c;
	RendererTextureStageRoute m_stageRoutes[10];             // 0x3490..0x34df
	RendererTextureOperationProfile m_operationProfiles[10]; // 0x34e0..0x35f7
};

DECOMP_SIZE_ASSERT(RendererTextureStageManager, 0x35f8);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureStageManager, m_recordIndexCache) == 0x2c80);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureStageManager, m_stageRoutes) == 0x3490);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureStageManager, m_operationProfiles) == 0x34e0);

#endif // LR2_RENDERERTEXTUREMANAGER_H
