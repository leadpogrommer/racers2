#include "foyersceneevents.h"

#include "charactervisual.h"
#include "foyeranimation.h"
#include "gamecontroller.h"
#include "gameobjectgeometry.h"
#include "gamerandom.h"
#include "matrix3.h"
#include "vector3.h"

#include <new>

namespace
{
LegoFloat NextFoyerFaceRandom()
{
	GameFloatRandom* random = &GetGameRandomState()->m_floatStreams[1];
	LegoU32 state;
	LegoFloat result;
	random->m_frame = random->m_multiplier * random->m_frame + random->m_increment;
	state = (random->m_mantissaMask & random->m_frame) | random->m_state;
	*(LegoU32*) &result = state;
	return result;
}

struct FoyerTransformDispatch {
	const void* m_vtable;
};

// Only the resource fields used by this closed event-update path are exposed.
struct FoyerEventResourcePrefix {
	LegoU8 m_opaque0x00[0x58];
	LegoU32 m_state;
	LegoU8 m_opaque0x5c[0x6c - 0x5c];
	FoyerSceneEventPrefix* m_events;
	LegoS32 m_count;
};
DECOMP_STATIC_ASSERT(offsetof(FoyerEventResourcePrefix, m_events) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(FoyerEventResourcePrefix, m_count) == 0x70);

typedef const LegoFloat*(__fastcall* FoyerGetVector)(void*, void*);
typedef void(__fastcall* FoyerSetVector)(void*, void*, const LegoFloat*);
} // namespace

// STUB: LEGORACERS2 0x0047c060
FoyerSceneEventPrefix* __fastcall InitializeFoyerSceneEvent(FoyerSceneEventPrefix* p_event, void*)
{
	p_event->m_unk0x00 = 0;
	p_event->m_unk0x04 = 0;
	new (&p_event->m_firstBuffer) CharBuffer24;
	p_event->m_unk0x24 = 0;
	new (&p_event->m_secondBuffer) CharBuffer24;
	return p_event;
}

// FUNCTION: LEGORACERS2 0x0047c260
void __fastcall SetFoyerSceneEventTarget(FoyerSceneEventPrefix* p_event, void*, void* p_target)
{
	p_event->m_unk0x00 = (LegoU32) p_target;
}

// FUNCTION: LEGORACERS2 0x0047c270
void __fastcall SetFoyerSceneEventReady(FoyerSceneEventPrefix* p_event, void*)
{
	p_event->m_unk0x24 = 1;
}

void __fastcall UpdateFoyerSceneEvents(FoyerSceneResourceView*, void*, LegoFloat);

// STUB: LEGORACERS2 0x0047c3d0
void __fastcall UpdateFoyerSceneEntry(FoyerSceneEventPrefix* p_event, void*, LegoFloat p_elapsedSeconds)
{
	LegoU32* ownerWords = (LegoU32*) p_event->m_unk0x00;
	FoyerAnimationControllerView* controller = *(FoyerAnimationControllerView**) ((LegoU8*) p_event->m_unk0x00 + 0x1e4);
	p_event->m_animationElapsed += p_elapsedSeconds;
	ownerWords[0x18c / 4] |= 0x4000;

	if (p_event->m_animationElapsed >= 0.4f) {
		if (p_event->m_animationElapsed < p_event->m_animationDuration) {
			ResetFoyerAnimationLayer(controller, NULL, p_event->m_animationLayer);
			if (p_event->m_hasPendingAnimation != 0) {
				p_event->m_hasPendingAnimation = 0;
				SetFoyerSceneEventAnimation(
					p_event,
					NULL,
					(const LegoChar*) &p_event->m_secondBuffer,
					*(LegoFloat*) &p_event->m_unk0x40,
					*(LegoFloat*) &p_event->m_unk0x44
				);
			}
		}
		else {
			SetFoyerSceneEventAnimation(p_event, NULL, p_event->m_firstBuffer.m_data, 1.0f, 3.402823466e+38f);
		}
	}
	else if (p_event->m_animationLayer == 1) {
		SetFoyerAnimationBlend(controller, NULL, p_event->m_animationElapsed * g_foyerAnimationTickScale);
	}
	else {
		SetFoyerAnimationBlend(controller, NULL, 1.0f - p_event->m_animationElapsed * g_foyerAnimationTickScale);
	}

	{
		LegoFloat value = g_foyerAnimationBlend * p_event->m_layerWeight0;
		SetFoyerAnimationLayerValue(controller, NULL, 0, *(LegoU32*) &value);
		value = g_foyerAnimationBlend * p_event->m_layerWeight1;
		SetFoyerAnimationLayerValue(controller, NULL, 1, *(LegoU32*) &value);
	}

	LegoU32 faceState = p_event->m_faceState;
	if (faceState != 10) {
		LegoFloat faceElapsed = p_event->m_faceElapsed + p_elapsedSeconds;
		p_event->m_faceElapsed = faceElapsed;
		if (faceElapsed >= p_event->m_faceDuration) {
			p_event->m_faceState = 10;
			((CharacterVisual*) p_event->m_unk0x08)->SetFaceVariant(0);
		}
		else if (faceState == 8 && faceElapsed > p_event->m_nextFaceTime) {
			LegoFloat random = NextFoyerFaceRandom();
			LegoU32 face = (!(random - 1.0f >= 0.65f)) ? 1 : 6;
			p_event->m_faceToggle = p_event->m_faceToggle == 0;
			((CharacterVisual*) p_event->m_unk0x08)->SetFaceVariant((LegoS32) (p_event->m_faceToggle * face));
			if (p_event->m_faceToggle == 0) {
				random = NextFoyerFaceRandom();
				p_event->m_nextFaceTime = (random - 1.0f) * 0.2f + p_event->m_nextFaceTime + 0.05f;
			}
			else {
				random = NextFoyerFaceRandom();
				p_event->m_nextFaceTime = (random - 1.0f) * 0.2f + p_event->m_nextFaceTime + 0.1f;
			}
		}
		else if (faceState == 9 && faceElapsed > p_event->m_nextFaceTime) {
			LegoFloat random = NextFoyerFaceRandom();
			LegoU32 face = (!(random - 1.0f >= 0.65f)) ? 1 : 4;
			p_event->m_faceToggle = p_event->m_faceToggle == 0;
			face = p_event->m_faceToggle ? 2 : face + 2;
			((CharacterVisual*) p_event->m_unk0x08)->SetFaceVariant((LegoS32) face);
			random = NextFoyerFaceRandom();
			p_event->m_nextFaceTime =
				(random - 1.0f) * 0.2f + p_event->m_nextFaceTime + ((p_event->m_faceToggle == 0) ? 0.05f : 0.1f);
		}
	}

	if (p_event->m_hasAttachedTarget != 0) {
		GameObjectChildGeometry* target = (GameObjectChildGeometry*) p_event->m_attachedTarget;
		FoyerTransformDispatch* ownerTransform = (FoyerTransformDispatch*) ((LegoU8*) p_event->m_unk0x00 + 0x58);
		const void* const* targetTable = *(const void* const**) target;
		const LegoFloat* rotation = ((FoyerGetVector) targetTable[7])(target, NULL);
		Matrix3 rotationMatrix;
		Vector3 position;
		const LegoFloat* targetPosition;
		const void* const* ownerTable = *(const void* const**) ownerTransform;
		position.m_x = p_event->m_attachedOffset[0];
		position.m_y = p_event->m_attachedOffset[1];
		position.m_z = p_event->m_attachedOffset[2];
		rotationMatrix.FUN_004a4d30(rotation);
		position.FUN_004a4e00(&rotationMatrix);
		targetPosition = ((FoyerGetVector) targetTable[5])(target, NULL);
		position.m_x += targetPosition[0];
		position.m_y += targetPosition[1];
		position.m_z += targetPosition[2];
		((FoyerSetVector) ownerTable[8])(ownerTransform, NULL, &position.m_x);
		((FoyerSetVector) ownerTable[9])(ownerTransform, NULL, rotation);
	}
}
// STUB: LEGORACERS2 0x0047c780
void __fastcall SetFoyerSceneEventAnimation(
	FoyerSceneEventPrefix* p_event,
	void*,
	const LegoChar* p_animationName,
	LegoFloat p_weight,
	LegoFloat p_duration
)
{
	LegoU32* copiedWords;
	FoyerAnimationControllerView* controller;
	FoyerAnimationLayerView* layer;
	LegoFloat duration;

	if (!(p_event->m_animationElapsed >= 0.4f)) {
		copiedWords = (LegoU32*) &p_event->m_secondBuffer;
		const LegoU32* sourceWords = (const LegoU32*) p_animationName;
		LegoS32 i;
		for (i = 0; i < 6; ++i) {
			copiedWords[i] = sourceWords[i];
		}
		p_event->m_unk0x40 = *(LegoU32*) &p_weight;
		p_event->m_unk0x44 = *(LegoU32*) &p_duration;
		p_event->m_hasPendingAnimation = 1;
		return;
	}

	controller = *(FoyerAnimationControllerView**) ((LegoU8*) p_event->m_unk0x00 + 0x1e4);
	StartFoyerAnimation(controller, NULL, p_animationName, p_event->m_animationLayer);
	SetFoyerAnimationBlend(controller, NULL, (p_event->m_animationLayer == 1) ? 1.0f : 0.0f);
	layer = controller->m_layers[p_event->m_animationLayer];
	// The retail x87 equality test also takes this branch for unordered values.
	if (p_duration == 0.0f || p_duration != p_duration) {
		const LegoU8* track = (const LegoU8*) layer->m_track;
		duration = *(const LegoFloat*) (track + 0x144) / p_weight - 0.4f;
	}
	else {
		duration = p_duration;
	}
	p_event->m_animationDuration = duration;
	p_event->m_animationElapsed = 0.0f;
	SetFoyerAnimationLayerValue(controller, NULL, p_event->m_animationLayer, *(LegoU32*) &p_weight);
	*(LegoFloat*) ((LegoU8*) p_event + 0x58 + p_event->m_animationLayer * sizeof(LegoFloat)) = p_weight;
	p_event->m_animationLayer = (p_event->m_animationLayer == 0) ? 1 : 0;
}

// STUB: LEGORACERS2 0x0047f6a0
LegoU32 __fastcall UpdateFoyerSceneHold(FoyerSceneResourceView* p_resourceView, void*, LegoFloat p_elapsedSeconds)
{
	FoyerEventResourcePrefix* resource = (FoyerEventResourcePrefix*) p_resourceView;
	LegoU32 state;

	UpdateFoyerSceneEvents(p_resourceView, NULL, p_elapsedSeconds);
	state = *(LegoU32*) ((LegoU8*) g_gameController.m_secondaryManager + 0x58);
	if (state != 2 && state != 3 && state != 4) {
		resource->m_state = 2;
	}
	return 1;
}

// STUB: LEGORACERS2 0x0047fe00
void __fastcall UpdateFoyerSceneEvents(FoyerSceneResourceView* p_resourceView, void*, LegoFloat p_elapsedSeconds)
{
	FoyerEventResourcePrefix* resource = (FoyerEventResourcePrefix*) p_resourceView;
	LegoS32 i;
	LegoU8* entry = (LegoU8*) resource->m_events;

	for (i = 0; i < resource->m_count; ++i, entry += 0x94) {
		UpdateFoyerSceneEntry((FoyerSceneEventPrefix*) entry, NULL, p_elapsedSeconds);
	}
}
