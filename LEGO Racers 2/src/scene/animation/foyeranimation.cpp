#include "scene/animation/foyeranimation.h"

// GLOBAL: LEGORACERS2 0x00572b30
LegoFloat g_foyerAnimationTickScale;

// GLOBAL: LEGORACERS2 0x00572b58
LegoFloat g_foyerAnimationBlend;

namespace
{
struct FoyerAnimationTrackRecordView {
	LegoU8 m_opaque0x00[0x104];
	LegoChar m_name[24];
};

void BindFoyerAnimationTrack(FoyerAnimationLayerView* p_layer, const void* p_track)
{
	p_layer->m_unk0x00 = 1;
	p_layer->m_track = p_track;
	p_layer->m_unk0x0c = 1.0f;
	p_layer->m_unk0x10 = 1.0f;
	p_layer->m_unk0x14 = 0;
}
} // namespace

// STUB: LEGORACERS2 0x0047c040
void __cdecl InitializeFoyerAnimationTiming()
{
	g_foyerAnimationTickScale = 1.0f / 0.4f;
}

// STUB: LEGORACERS2 0x004c5560
LegoS32 __fastcall FindFoyerAnimationTrack(
	FoyerAnimationControllerView* p_controller,
	void*,
	const LegoChar* p_name,
	LegoU32
)
{
	CharBuffer24 name(p_name);
	LegoS32 i;
	const LegoS32* trackIds = p_controller->m_trackIds;
	const LegoU8* trackRecords = (const LegoU8*) p_controller->m_trackRecords;

	for (i = 0; i < p_controller->m_trackCount; ++i) {
		LegoS32 trackId = trackIds[i];
		const LegoChar* candidate = (const LegoChar*) (trackRecords + trackId * 0x148 + 0x104);
		LegoU32 j;
		for (j = 0; j < 24; ++j) {
			if (candidate[j] != name.m_data[j]) {
				break;
			}
			if (candidate[j] == '\0') {
				return trackId;
			}
		}
		if (j == 24) {
			return trackId;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x004c55f0
void __fastcall StartFoyerAnimation(
	FoyerAnimationControllerView* p_controller,
	void*,
	const LegoChar* p_name,
	LegoU32 p_layerIndex
)
{
	LegoS32 trackId = FindFoyerAnimationTrack(p_controller, NULL, p_name, 1);
	const LegoU8* track = (const LegoU8*) p_controller->m_trackRecords + trackId * 0x148;
	BindFoyerAnimationTrack(p_controller->m_layers[p_layerIndex], track + 4);
}

// STUB: LEGORACERS2 0x004c5640
void __fastcall ApplyFoyerAnimationTrack(
	FoyerAnimationControllerView* p_controller,
	void*,
	LegoU32 p_trackId,
	LegoU32 p_layerIndex
)
{
	const LegoU8* track = (const LegoU8*) p_controller->m_trackRecords + p_trackId * 0x148;
	BindFoyerAnimationTrack(p_controller->m_layers[p_layerIndex], track + 4);
}

// FUNCTION: LEGORACERS2 0x004c5680
void __fastcall ResetFoyerAnimationLayer(FoyerAnimationControllerView* p_controller, void*, LegoU32 p_layerIndex)
{
	p_controller->m_layers[p_layerIndex]->m_unk0x00 = 0;
}

// FUNCTION: LEGORACERS2 0x004c56a0
void __fastcall SetFoyerAnimationLayerValues(
	FoyerAnimationControllerView* p_controller,
	void*,
	LegoU32 p_layerIndex,
	LegoU32 p_value0x0c,
	LegoU32 p_value0x10,
	LegoU32
)
{
	FoyerAnimationLayerView* layer = p_controller->m_layers[p_layerIndex];
	layer->m_unk0x0c = *(LegoFloat*) &p_value0x0c;
	layer->m_unk0x10 = *(LegoFloat*) &p_value0x10;
}

// FUNCTION: LEGORACERS2 0x004c56c0
void __fastcall SetFoyerAnimationLayerValue(
	FoyerAnimationControllerView* p_controller,
	void*,
	LegoU32 p_layerIndex,
	LegoU32 p_value
)
{
	p_controller->m_layers[p_layerIndex]->m_unk0x0c = *(LegoFloat*) &p_value;
}

// FUNCTION: LEGORACERS2 0x004c56e0
void __fastcall SetFoyerAnimationLayerValueReordered(
	FoyerAnimationControllerView* p_controller,
	void*,
	LegoU32 p_value,
	LegoU32 p_layerIndex
)
{
	p_controller->m_layers[p_layerIndex]->m_unk0x10 = *(LegoFloat*) &p_value;
}

// FUNCTION: LEGORACERS2 0x004c5700
LegoFloat __fastcall GetFoyerAnimationLayerValue(
	FoyerAnimationControllerView* p_controller,
	void*,
	LegoU32 p_layerIndex
)
{
	return p_controller->m_layers[p_layerIndex]->m_unk0x10;
}

// FUNCTION: LEGORACERS2 0x004c5720
void __fastcall SetFoyerAnimationBlend(FoyerAnimationControllerView* p_controller, void*, LegoFloat p_blend)
{
	p_controller->m_layers[0]->m_unk0x10 = p_blend;
	p_controller->m_layers[1]->m_unk0x10 = 1.0f - p_blend;
}

struct InitializeFoyerAnimationTimingAtStartup {
	InitializeFoyerAnimationTimingAtStartup() { InitializeFoyerAnimationTiming(); }
};
static InitializeFoyerAnimationTimingAtStartup g_initializeFoyerAnimationTimingAtStartup;
