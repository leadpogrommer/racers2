#include "controllerloadingprogress.h"

#include "controllereventrecords.h"
#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gameobjectrenderprep.h"
#include "gamerandom.h"
#include "globaloverlaymanager.h"
#include "globalpause.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "secondaryfxmanager.h"
#include "secondarymanager.h"

#include <math.h>
#include <string.h>

namespace
{
LegoFloat NextLoadingRandom()
{
	GameFloatRandom* stream = &GetGameRandomState()->m_floatStreams[1];
	stream->m_frame = stream->m_multiplier * stream->m_frame + stream->m_increment;
	union {
		LegoU32 m_bits;
		LegoFloat m_float;
	} value;
	value.m_bits = (stream->m_mantissaMask & stream->m_frame) | stream->m_state;
	return value.m_float - 1.0f;
}

void CopyLoadingTexture(GlobalOverlaySprite* p_sprite, const ControllerEventRecord* p_record)
{
	memcpy(&p_sprite->m_texture, p_record, sizeof(ControllerEventRecord));
}
} // namespace

// STUB: LEGORACERS2 0x004338f0
void __fastcall InitializeControllerSelectionProgress(void* p_storage, void*, LegoU32 p_frontend)
{
	ControllerLoadingProgress* state = (ControllerLoadingProgress*) p_storage;
	state->m_active = 1;
	state->m_mode = p_frontend != 0 ? -1 : g_gameControllerSession->m_selectedMode;
	SecondaryFxState* effects = ((SecondaryFxManager*) g_gameController.m_secondaryFxManager)->m_state;
	effects->SetMode(0, 0.0f);
	state->m_progress = 0.0f;
	state->m_elapsed = 0.0f;
	for (LegoS32 index = 0; index < 5; ++index) {
		LegoFloat value = NextLoadingRandom();
		state->m_targets[index] = value;
		state->m_values[index] = value;
		state->m_velocities[index] = 0.0f;
	}
	LoadControllerLoadingTextures(g_gameController.m_unk0x90, NULL, state->m_mode);
	CreateControllerLoadingSprites(state, NULL);
}

// STUB: LEGORACERS2 0x004339a0
void __fastcall FinalizeControllerSelectionProgress(void* p_storage, void*)
{
	ControllerLoadingProgress* state = (ControllerLoadingProgress*) p_storage;
	state->m_active = 0;
	for (LegoS32 index = 17; index >= 0; --index) {
		if (state->m_sprites[index] != NULL) {
			state->m_sprites[index]->Release();
			state->m_sprites[index] = NULL;
		}
	}
	ReleaseControllerLoadingTextures(g_gameController.m_unk0x90, NULL, state->m_mode);
}

// STUB: LEGORACERS2 0x004339e0
void __fastcall UpdateControllerSelectionProgress(void* p_storage, void*, LegoU32 p_force)
{
	ControllerLoadingProgress* state = (ControllerLoadingProgress*) p_storage;
	if (state->m_active != 0) {
		LegoFloat time = ReadSignedClockSeconds();
		LegoFloat delta;
		if (p_force != 0) {
			delta = 0.0f;
		}
		else {
			delta = time - state->m_lastUpdate;
			if (!(delta > 0.0333333351f)) {
				return;
			}
		}
		state->m_lastUpdate = time;
		state->m_elapsed += delta;
		AdvanceControllerLoadingEqualizer(state, NULL, delta);
		UpdateControllerLoadingSprites(state, NULL);
		const RendererMainDispatch* dispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
		if (dispatch->m_beginScene(g_displayRenderer, NULL) == 1) {
			PrepareGameObjectForRendering(((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x110, NULL);
			RenderGlobalOverlayPass(&g_globalOverlayManager, NULL, 0);
			dispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
			dispatch->m_endScene(g_displayRenderer, NULL);
			dispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
			dispatch->m_present(g_displayRenderer, NULL);
			dispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
			dispatch->m_clearBuffers(g_displayRenderer, NULL, 7);
		}
	}
}

// STUB: LEGORACERS2 0x00433aa0
void __fastcall AdvanceControllerLoadingEqualizer(ControllerLoadingProgress* p_state, void*, LegoFloat p_delta)
{
	for (LegoS32 index = 0; index < 5; ++index) {
		LegoFloat* target = &p_state->m_targets[index];
		LegoFloat* value = &p_state->m_values[index];
		if (*value == *target) {
			LegoFloat distance = NextLoadingRandom() * 0.15f + 0.05f;
			LegoFloat direction;
			if (*target > 0.99f) {
				direction = -1.0f;
			}
			else if (*target < 0.01f) {
				direction = 1.0f;
			}
			else {
				GameIntegerRandom* stream = &GetGameRandomState()->m_integerStreams[1];
				stream->m_state = stream->m_multiplier * stream->m_state + stream->m_increment;
				direction = (stream->m_state & 0x1000000) != 0 ? -1.0f : 1.0f;
			}
			distance *= direction;
			LegoFloat candidate = distance + *target;
			*target = candidate;
			if (candidate > 1.0f) {
				distance = (1.0f - candidate) + distance;
				*target = 1.0f;
			}
			else if (candidate < 0.0f) {
				distance -= candidate;
				*target = 0.0f;
			}
			p_state->m_velocities[index] = distance / (NextLoadingRandom() * 0.15f + 0.05f);
		}
		LegoFloat step = p_delta * p_state->m_velocities[index];
		if (fabs(*target - *value) < fabs(step)) {
			*value = *target;
		}
		else {
			*value += step;
		}
	}
}

// STUB: LEGORACERS2 0x00433c50
void __fastcall CreateControllerLoadingSprites(ControllerLoadingProgress* p_state, void*)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	LegoU32 parameters[21];
	parameters[0] = parameters[1] = parameters[2] = parameters[3] = 0;
	parameters[4] = parameters[5] = 0;
	parameters[6] = parameters[7] = 0x3f800000;
	parameters[10] = parameters[11] = 0xffffffff;
	parameters[12] = parameters[15] = 0;
	parameters[13] = parameters[14] = 0x3f800000;
	parameters[16] = parameters[17] = parameters[18] = parameters[19] = 0;
	parameters[20] = 6;
	LegoFloat halfSize[2] = {0.5f, 0.5f};
	LegoS32 index;
	for (index = 0; index < 4; ++index) {
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		p_state->m_sprites[index] = sprite;
		CopyLoadingTexture(sprite, &records->m_records[270 + index]);
		LegoFloat position[2] = {(index & 1) * halfSize[0], ((index >> 1) & 1) * halfSize[1]};
		sprite->SetPosition(position);
		sprite->SetSize(halfSize);
		sprite->SetVisible(1);
	}
	parameters[20] = 7;
	LegoFloat origin[2] = {0.0f, 0.0f};
	for (index = 0; index < 3; ++index) {
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		p_state->m_sprites[4 + index] = sprite;
		CopyLoadingTexture(sprite, &records->m_records[274 + index]);
		sprite->SetPosition(origin);
		sprite->SetSize(g_controllerLoadingTuning.m_progressSize);
		sprite->SetVisible(1);
	}
	parameters[20] = 8;
	GlobalOverlaySprite* word = g_globalOverlayManager.AllocateSprite(parameters);
	p_state->m_sprites[7] = word;
	CopyLoadingTexture(word, &records->m_records[277]);
	LegoFloat position[2];
	position[0] =
		(0.68f * 0.5f + g_controllerLoadingTuning.m_progressSize[0] + g_controllerLoadingTuning.m_progressOrigin[0]) -
		g_controllerLoadingTuning.m_wordSize[0] * 0.5f;
	position[1] = (g_controllerLoadingTuning.m_progressSize[1] * 0.5f + g_controllerLoadingTuning.m_progressOrigin[1]) -
				  g_controllerLoadingTuning.m_wordSize[1] * 0.5f;
	word->SetPosition(position);
	word->SetSize(g_controllerLoadingTuning.m_wordSize);
	word->SetVisible(1);
	parameters[20] = 7;
	for (index = 0; index < 5; ++index) {
		for (LegoS32 part = 0; part < 2; ++part) {
			GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
			p_state->m_sprites[8 + index * 2 + part] = sprite;
			CopyLoadingTexture(sprite, &records->m_records[278 + part]);
			sprite->SetVisible(1);
		}
		UpdateControllerLoadingEqualizerBar(p_state, NULL, index, p_state->m_values[index]);
	}
}

// STUB: LEGORACERS2 0x00433fc0
void __fastcall UpdateControllerLoadingSprites(ControllerLoadingProgress* p_state, void*)
{
	LegoFloat position[2];
	position[0] = g_controllerLoadingTuning.m_progressOrigin[0];
	position[1] = g_controllerLoadingTuning.m_progressOrigin[1];
	p_state->m_sprites[4]->SetPosition(position);
	position[0] += g_controllerLoadingTuning.m_progressSize[0];
	LegoFloat size[2] = {0.68f * p_state->m_progress, g_controllerLoadingTuning.m_progressSize[1]};
	p_state->m_sprites[5]->SetPosition(position);
	p_state->m_sprites[5]->SetSize(size);
	position[0] += size[0];
	p_state->m_sprites[6]->SetPosition(position);
	for (LegoS32 index = 0; index < 5; ++index) {
		UpdateControllerLoadingEqualizerBar(p_state, NULL, index, p_state->m_values[index]);
	}
}

// STUB: LEGORACERS2 0x00434060
void __fastcall UpdateControllerLoadingEqualizerBar(
	ControllerLoadingProgress* p_state,
	void*,
	LegoS32 p_index,
	LegoFloat p_value
)
{
	LegoFloat size[2];
	size[0] = g_controllerLoadingTuning.m_equalizerSize[0];
	size[1] = (g_controllerLoadingTuning.m_equalizerSize[1] - 0.015625f) * p_value;
	LegoFloat position[2];
	position[0] = (g_controllerLoadingTuning.m_equalizerSize[0] + 0.01171875f) * p_index +
				  g_controllerLoadingTuning.m_equalizerOrigin[0];
	position[1] = g_controllerLoadingTuning.m_equalizerOrigin[1] - size[1];
	GlobalOverlaySprite* body = p_state->m_sprites[9 + p_index * 2];
	body->SetPosition(position);
	body->SetSize(size);
	LegoFloat capSize[2] = {g_controllerLoadingTuning.m_equalizerSize[0], 0.015625f};
	position[1] -= capSize[1];
	GlobalOverlaySprite* cap = p_state->m_sprites[8 + p_index * 2];
	cap->SetPosition(position);
	cap->SetSize(capSize);
}

// FUNCTION: LEGORACERS2 0x00502a10
void __fastcall SetControllerSelectionProgressValue(void* p_storage, void*, LegoFloat p_value)
{
	((ControllerLoadingProgress*) p_storage)->m_progress = p_value;
}
