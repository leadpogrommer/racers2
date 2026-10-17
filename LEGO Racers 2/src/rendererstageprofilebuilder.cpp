#include "renderer.h"
#include "rendererstagebuilders.h"
#include "rendererstageplan.h"

namespace
{

struct StageProfileEntry {
	LegoU32 m_words[4];
};

DECOMP_SIZE_ASSERT(StageProfileEntry, 0x10);

static const StageProfileEntry g_profile21Primary[7] =
	{{{1, 1, 2, 0}}, {{1, 2, 2, 0}}, {{1, 4, 2, 0}}, {{1, 5, 2, 0}}, {{1, 0x12, 2, 0}}, {{1, 1, 1, 1}}, {{1, 4, 1, 1}}};

static const StageProfileEntry g_profile21Secondary[7] = {
	{{1, 1, 4, 0}},
	{{1, 2, 2, 0}},
	{{1, 3, 0, 0}},
	{{0, 0x1b, 1, 0}},
	{{0, 0x0f, 0, 0}},
	{{0, 0x13, 1, 0}},
	{{0, 0x14, 3, 0}}
};

static const StageProfileEntry g_profile3Primary[9] = {
	{{1, 1, 4, 0}},
	{{1, 2, 2, 0}},
	{{1, 3, 0, 0}},
	{{1, 4, 4, 0}},
	{{1, 5, 2, 0}},
	{{1, 6, 0, 0}},
	{{1, 0x12, 2, 0}},
	{{1, 1, 1, 1}},
	{{1, 4, 1, 1}}
};

static const StageProfileEntry g_profile3Secondary[5] =
	{{{1, 0x18, 2, 0}}, {{0, 0x1b, 1, 0}}, {{0, 0x0f, 0, 0}}, {{0, 0x13, 2, 0}}, {{0, 0x14, 2, 0}}};

static const StageProfileEntry g_profile3Tertiary[1] = {{{1, 0x18, 0, 0}}};

static const StageProfileEntry g_profile11Primary[9] = {
	{{1, 1, 4, 0}},
	{{1, 2, 2, 0}},
	{{1, 3, 0, 0}},
	{{1, 4, 4, 0}},
	{{1, 5, 2, 0}},
	{{1, 6, 0, 0}},
	{{1, 0x12, 2, 0}},
	{{1, 1, 1, 1}},
	{{1, 4, 1, 1}}
};

static const StageProfileEntry g_profile11Secondary[4] =
	{{{0, 0x1b, 1, 0}}, {{0, 0x0f, 0, 0}}, {{0, 0x13, 2, 0}}, {{0, 0x14, 2, 0}}};

static const StageProfileEntry g_profile101Primary[8] = {
	{{1, 1, 4, 0}},
	{{1, 2, 2, 0}},
	{{1, 3, 0, 0}},
	{{1, 4, 2, 0}},
	{{1, 5, 2, 0}},
	{{1, 0x12, 2, 0}},
	{{1, 1, 1, 1}},
	{{1, 4, 1, 1}}
};

static const StageProfileEntry g_profile101Secondary[5] =
	{{{1, 1, 2, 0}}, {{0, 0x1b, 1, 0}}, {{0, 0x0f, 0, 0}}, {{0, 0x13, 1, 0}}, {{0, 0x14, 4, 0}}};

static const StageProfileEntry g_profile201Primary[9] = {
	{{1, 1, 4, 0}},
	{{1, 2, 2, 0}},
	{{1, 3, 0, 0}},
	{{1, 4, 4, 0}},
	{{1, 5, 2, 0}},
	{{1, 6, 0, 0}},
	{{1, 0x12, 2, 0}},
	{{1, 1, 1, 1}},
	{{1, 4, 1, 1}}
};

static const StageProfileEntry g_profile201Secondary[4] =
	{{{0, 0x1b, 1, 0}}, {{0, 0x0f, 0, 0}}, {{0, 0x13, 2, 0}}, {{0, 0x14, 2, 0}}};

static const StageProfileEntry g_profile201Tertiary[1] = {{{1, 0x18, 0, 0}}};

static const StageProfileEntry g_profile205Primary[15] = {
	{{1, 1, 4, 0}},
	{{1, 2, 2, 0}},
	{{1, 3, 0, 0}},
	{{1, 4, 2, 0}},
	{{1, 5, 2, 0}},
	{{1, 0x12, 2, 0}},
	{{1, 1, 0x12, 1}},
	{{1, 2, 2, 1}},
	{{1, 3, 1, 1}},
	{{1, 4, 3, 1}},
	{{1, 5, 2, 1}},
	{{1, 6, 1, 1}},
	{{1, 0x12, 2, 1}},
	{{1, 1, 1, 1}},
	{{1, 2, 4, 2}}
};

static const StageProfileEntry g_profile205Secondary[1] = {{{1, 0x18, 0, 1}}};

void AddStageRecords(
	RendererTextureStageManager* p_manager,
	RendererSubrecordGroup* p_group,
	LegoU32 p_groupIndex,
	const StageProfileEntry* p_entries,
	LegoS32 p_count
)
{
	AddTextureStageSubrecords(
		p_manager,
		NULL,
		p_group->m_records,
		p_groupIndex,
		(const RendererArrayMarker*) p_entries,
		p_count
	);
}

LegoU32 ApplyBuiltStagePlan(
	RendererTextureStageManager* p_manager,
	RendererSubrecordGroup* p_group,
	RendererStagePlanRequest* p_request,
	LegoU32 p_mode
)
{
	return ApplyTextureStagePlanForManager(p_manager, NULL, p_group, p_request, p_mode);
}

LegoU32 BuildThreeStageProfile(RendererTextureStageManager* p_manager, RendererStagePlanRequest* p_request)
{
	RendererSubrecordGroup* group = p_manager->CreateSubrecordGroup(p_request->m_defaults.m_unk0x00);
	group->m_unk0xc0 = 3;
	AddStageRecords(p_manager, group, 0, g_profile3Primary, 9);
	AddStageRecords(p_manager, group, 1, g_profile3Secondary, 5);
	AddStageRecords(p_manager, group, 2, g_profile3Tertiary, 1);
	SetTextureStageProfileValue((TextureStageProfileIndexMapView*) &group->m_records[0], NULL, 0, 0);
	SetTextureStageProfileValue((TextureStageProfileIndexMapView*) &group->m_records[1], NULL, 1, 0);
	group->m_records[2].m_unk0x2c = 0;
	return ApplyBuiltStagePlan(p_manager, group, p_request, 1);
}

} // namespace

// STUB: LEGORACERS2 0x004e4fd0
LegoU32 RendererTextureStageManager::BuildProfileStagePlan(RendererStagePlanRequest* p_request, LegoU32 p_unk0x04)
{
	(void) p_unk0x04;

	const LegoU32 profile = p_request->m_defaults.m_unk0x00;
	DisplayRenderer* renderer = (DisplayRenderer*) ((BYTE*) this - offsetof(DisplayRenderer, m_textureStageManager));
	RendererSubrecordGroup* group;

	switch (profile) {
	case 0x21:
		group = CreateSubrecordGroup(profile);
		group->m_unk0xc0 = 2;
		AddStageRecords(this, group, 0, g_profile21Primary, 7);
		AddStageRecords(this, group, 1, g_profile21Secondary, 7);
		group->m_records[0].m_unk0x00[0] = 0;
		if ((LegoS32) group->m_records[0].m_unk0x28 < 0) {
			group->m_records[0].m_unk0x28 = 0;
		}
		group->m_records[1].m_unk0x00[0] = 5;
		if ((LegoS32) group->m_records[1].m_unk0x28 < 0) {
			group->m_records[1].m_unk0x28 = 0;
		}
		return ApplyBuiltStagePlan(this, group, p_request, 0);

	case 3:
		return BuildThreeStageProfile(this, p_request);

	case 0x11:
		group = CreateSubrecordGroup(profile);
		group->m_unk0xc0 = 2;
		AddStageRecords(this, group, 0, g_profile11Primary, 9);
		AddStageRecords(this, group, 1, g_profile11Secondary, 4);
		group->m_records[0].m_unk0x00[0] = 0;
		if ((LegoS32) group->m_records[0].m_unk0x28 < 0) {
			group->m_records[0].m_unk0x28 = 0;
		}
		group->m_records[1].m_unk0x00[0] = 4;
		if ((LegoS32) group->m_records[1].m_unk0x28 < 0) {
			group->m_records[1].m_unk0x28 = 0;
		}
		return ApplyBuiltStagePlan(this, group, p_request, 1);

	case 0x101:
		group = CreateSubrecordGroup(profile);
		group->m_unk0xc0 = 2;
		AddStageRecords(this, group, 0, g_profile101Primary, 8);
		AddStageRecords(this, group, 1, g_profile101Secondary, 5);
		group->m_records[0].m_unk0x00[0] = 0;
		if ((LegoS32) group->m_records[0].m_unk0x28 < 0) {
			group->m_records[0].m_unk0x28 = 0;
		}
		group->m_records[1].m_unk0x00[0] = 8;
		if ((LegoS32) group->m_records[1].m_unk0x28 < 0) {
			group->m_records[1].m_unk0x28 = 0;
		}
		return ApplyBuiltStagePlan(this, group, p_request, 0);

	case 0x201: {
		group = CreateSubrecordGroup(profile);
		group->m_unk0xc0 = 3;
		AddStageRecords(this, group, 0, g_profile201Primary, 9);
		AddStageRecords(this, group, 1, g_profile201Secondary, 4);
		AddStageRecords(this, group, 2, g_profile201Tertiary, 1);
		group->m_records[0].m_unk0x00[0] = 0;
		if ((LegoS32) group->m_records[0].m_unk0x28 < 0) {
			group->m_records[0].m_unk0x28 = 0;
		}
		group->m_records[1].m_unk0x00[0] = 9;
		if ((LegoS32) group->m_records[1].m_unk0x28 < 0) {
			group->m_records[1].m_unk0x28 = 0;
		}
		group->m_records[2].m_unk0x2c = 0;

		const LegoU32 recordIndex = renderer->SupportsCubeTextures() == 0 ? 2 : 3;
		const LegoU32 index = m_unk0x3484++;
		RendererStageRecordLink* entry = (RendererStageRecordLink*) &m_stageRecords[index];
		InitializeAndLinkTextureStageRecord(&group->m_records[2], NULL, entry, 0, 0x18, recordIndex);
		return ApplyBuiltStagePlan(this, group, p_request, 1);
	}

	case 0x205:
		if (renderer->SupportsSharedMemoryMultitexturing() == 0) {
			return BuildThreeStageProfile(this, p_request);
		}
		group = CreateSubrecordGroup(profile);
		group->m_unk0xc0 = 2;
		AddStageRecords(this, group, 0, g_profile205Primary, 15);
		AddStageRecords(this, group, 1, g_profile205Secondary, 1);
		group->m_records[0].m_unk0x00[0] = 0;
		group->m_records[0].m_unk0x00[1] = 9;
		if ((LegoS32) group->m_records[0].m_unk0x28 < 1) {
			group->m_records[0].m_unk0x28 = 1;
		}
		group->m_records[1].m_unk0x2c = 0;

		const LegoU32 recordIndex = renderer->SupportsCubeTextures() == 0 ? 2 : 3;
		const LegoU32 index = m_unk0x3484++;
		RendererStageRecordLink* entry = (RendererStageRecordLink*) &m_stageRecords[index];
		InitializeAndLinkTextureStageRecord(&group->m_records[1], NULL, entry, 1, 0x18, recordIndex);
		return ApplyBuiltStagePlan(this, group, p_request, 0);
	}

	return 0;
}
