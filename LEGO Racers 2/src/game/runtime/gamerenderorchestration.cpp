#include "core/application/application.h"
#include "core/debug/profiler.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "game/objects/gamemanager.h"
#include "game/objects/gameobjectrenderprep.h"
#include "game/runtime/gamerender.h"
#include "game/runtime/gamerenderaudio.h"
#include "game/runtime/gamerenderframe.h"
#include "render/buffers/rendererbuffer.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/draw/rendererquaddraw.h"
#include "render/resources/rendererrecords.h"

#include <string.h>

namespace
{

void ClearRendererLookupTables(DisplayRenderer* p_renderer)
{
	for (LegoU32 index = 0; index < 3; ++index) {
		p_renderer->m_drawLookupTables[index]->Clear();
	}
}

void FlushRendererLookupTable(DisplayRenderer* p_renderer, LegoU32 p_index)
{
	RendererLookupTableState* table = p_renderer->m_drawLookupTables[p_index];
	const RendererLookupDispatch* dispatch = (const RendererLookupDispatch*) table->m_vtable;
	dispatch->m_flush(table, NULL);
}

void ConfigureObjectStencil(DisplayRenderer* p_renderer, LegoU32 p_function, LegoU32 p_operation)
{
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) p_renderer->m_vtable;
	dispatch->m_setStencilBufferEnabled(p_renderer, NULL, 1);
	dispatch->m_setStencilFunction(p_renderer, NULL, p_function);
	dispatch->m_setStencilOperation(p_renderer, NULL, 2, p_operation, 0);
}

void ConfigureSecondaryStencil(DisplayRenderer* p_renderer)
{
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) p_renderer->m_vtable;
	dispatch->m_setStencilBufferEnabled(p_renderer, NULL, 1);
	dispatch->m_setStencilFunction(p_renderer, NULL, 7);
	dispatch->m_setStencilOperation(p_renderer, NULL, 2, 6, 0);
	dispatch->m_setStencilOperation(p_renderer, NULL, 1, 6, 0);
}

void DrawGameObject(GameObject* p_object)
{
	p_object->m_vtable->m_action0x30(p_object, NULL);
}

} // namespace

// STUB: LEGORACERS2 0x004aa8e0
void GameManager::RenderFrame(LegoFloat p_frameDelta)
{
	DisplayRenderer* renderer = g_displayRenderer;
	RendererMainDispatch* dispatch = (RendererMainDispatch*) renderer->m_vtable;

	if (g_gameLifecycleHooksNeedRefresh != 0) {
		if (g_unk0x0067bafc != NULL) {
			for (LegoS32 index = 0; index < g_gameObjectCount; ++index) {
				GameObject* object = g_gameObjects[index];
				object->m_vtable->m_action0x48(object, NULL, (LegoU32) g_unk0x0067bafc);
			}
		}
		g_gameLifecycleHooksNeedRefresh = 0;
	}

	CharBuffer24 beginRenderingName("Begin Rendering");
	void* beginRenderingRange = BeginProfileRange(&beginRenderingName);
	LegoS32 beganScene = dispatch->m_beginScene(renderer, NULL);
	EndProfileRange(beginRenderingRange);

	CharBuffer24 renderingTimeName("Rendering Time");
	void* renderingTimeRange = BeginProfileRange(&renderingTimeName);
	UpdateComponents(p_frameDelta);

	if (beganScene == 1) {
		LegoU32 firstRenderableType3 = 0xffffffff;
		for (LegoS32 index = 0; index < g_gameObjectCount; ++index) {
			GameObject* object = g_gameObjects[index];
			LegoU32 typePrefix = object->m_type & 0xffffff00;
			if ((LegoS32) typePrefix < 0x300) {
				continue;
			}
			if ((object->m_type >> 8) != 3) {
				break;
			}
			if (*(LegoU32*) ((LegoU8*) object + 0x1c0) == 0 || (object->m_flags & 1) != 0) {
				continue;
			}
			if (firstRenderableType3 == 0xffffffff) {
				firstRenderableType3 = (LegoU32) index;
			}

			PrepareGameObjectForRendering(object, NULL);
			if (*(LegoU32*) ((LegoU8*) object + 0x1cc) != 0) {
				ClearRendererLookupTables(renderer);
				for (LegoS32 objectIndex = 0; objectIndex < g_gameObjectCount; ++objectIndex) {
					GameObject* secondary = g_gameObjects[objectIndex];
					LegoS32 type = ((LegoS32) secondary->m_type) >> 8;
					if (type > 2) {
						break;
					}
					if (type == 2) {
						ForwardPositionToChildGeometry(secondary, NULL, (const LegoFloat*) ((LegoU8*) object + 0x188));
					}
				}
				LegoS32 secondaryPassStart = 0;
				LegoS32 secondaryObjectCount = g_gameObjectCount;
				if (dispatch->m_isSolidPassOverrideActive(renderer, NULL) != 0) {
					GameObject* selectedSecondary = g_gameObjects[0];
					if (secondaryObjectCount > 0) {
						for (secondaryPassStart = 0; secondaryPassStart < secondaryObjectCount; ++secondaryPassStart) {
							GameObject* secondary = g_gameObjects[secondaryPassStart];
							selectedSecondary = secondary;
							if (((secondary->m_flags >> 1) & 8) == 0 && (secondary->m_flags & 8) == 0 &&
								*(LegoU32*) ((LegoU8*) secondary + 0x48) != 0) {
								break;
							}
						}
					}
					dispatch->m_setDepthAlwaysWrite(renderer, NULL);
					ConfigureObjectStencil(renderer, 7, 1);
					DrawGameObject(selectedSecondary);
					dispatch->m_applyTextureOperation(renderer, NULL, 0);
					renderer->m_unk0x4c8 = 0;
					renderer->m_unk0x4cc = NULL;
					FlushRendererLookupTable(renderer, 1);
					if (renderer->m_unk0x4c8 != 0) {
						dispatch->m_applyTextureOperation(renderer, NULL, 0x0e);
					}
					dispatch->m_setStencilBufferEnabled(renderer, NULL, 0);
					dispatch->m_setDepthState(renderer, NULL, 1);
					++secondaryPassStart;
				}
				if (GetApplication()->GetUnknownFlag() != 0) {
					ConfigureSecondaryStencil(renderer);
				}
				GameObject** secondary = g_gameObjects + secondaryPassStart;
				GameObject** secondaryEnd = g_gameObjects + secondaryObjectCount;
				do {
					GameObject* secondaryObject = *secondary;
					if (((secondaryObject->m_flags >> 1) & 8) == 0 && (secondaryObject->m_flags & 8) == 0) {
						DrawGameObject(secondaryObject);
					}
					++secondary;
				} while (secondary != secondaryEnd);
				dispatch->m_applyTextureOperation(renderer, NULL, 0);
				renderer->m_unk0x4c8 = 0;
				renderer->m_unk0x4cc = NULL;
				FlushRendererLookupTable(renderer, 1);
				if (renderer->m_unk0x4c8 != 0) {
					dispatch->m_applyTextureOperation(renderer, NULL, 0x0e);
				}
				dispatch->m_noOp014(renderer, NULL);

				dispatch->m_applyTextureOperation(renderer, NULL, 0);
				renderer->m_unk0x4c8 = 0;
				renderer->m_unk0x4cc = NULL;
				FlushRendererLookupTable(renderer, 2);
				if (renderer->m_unk0x4c8 != 0) {
					dispatch->m_applyTextureOperation(renderer, NULL, 0x0e);
				}
				dispatch->m_noOp014(renderer, NULL);

				dispatch->m_applyTextureOperation(renderer, NULL, 0);
				renderer->m_unk0x4c8 = 0;
				renderer->m_unk0x4cc = NULL;
				FlushRendererLookupTable(renderer, 0);
				if (renderer->m_unk0x4c8 != 0) {
					dispatch->m_applyTextureOperation(renderer, NULL, 0x0e);
				}
			}
			else if (GetApplication()->GetUnknownFlag() != 0) {
				ConfigureSecondaryStencil(renderer);
			}
			dispatch->m_noOp018(renderer, NULL);
			if (*(LegoS32*) ((LegoU8*) object + 0x1c8) != -1) {
				RenderGlobalOverlayPass(&g_globalOverlayManager, NULL, 0);
			}
			if (GetApplication()->GetUnknownFlag() != 0) {
				// Retail keeps these two draw packets adjacent on the stack: the
				// first rectangle begins one DWORD into the storage used by the
				// second rectangle, and the defaults record starts immediately
				// after the first rectangle. Keep the byte overlap explicit.
				LegoU32 overlayStorage[31];
				ZeroMemory(overlayStorage, sizeof(overlayStorage));
				LegoU32* stripeWords = &overlayStorage[1];
				LegoU32* colorWords = &overlayStorage[0];
				RendererDefaultRecord* defaults = (RendererDefaultRecord*) &overlayStorage[21];

				// Stack-record initialization from 0x004aad09..0x004aae83.
				stripeWords[4] = 0xffc00000;
				stripeWords[7] = 0x3f800000;
				stripeWords[8] = 0x3f800000;
				stripeWords[16] = 0x3f800000;
				stripeWords[17] = 0x3f800000;
				LegoU32* defaultsWords = (LegoU32*) defaults;
				defaultsWords[0] = 0xffffffff;
				defaultsWords[1] = 0xffc00000;
				defaultsWords[4] = 0x3f800000;
				defaultsWords[5] = 0x3f800000;

				// The other rectangle is the preceding 0x54 bytes. It shares its
				// trailing fields with the first rectangle and defaults record.
				colorWords[3] = 0x3f800000;
				colorWords[4] = 0x3f800000;
				colorWords[5] = 0xffc00000;
				colorWords[7] = 0x3f800000;
				colorWords[8] = 0x3f800000;
				colorWords[9] = 0x3f800000;

				dispatch->m_setDepthState(renderer, NULL, 0);
				const LegoFloat step = 0.02349624f;
				for (LegoS32 row = 0; row < 32; ++row) {
					LegoFloat lower = (LegoFloat) row * step;
					LegoFloat upper = (LegoFloat) (row + 1) * step;
					memcpy(&stripeWords[10], &lower, sizeof(lower));
					memcpy(&stripeWords[11], &step, sizeof(step));
					memcpy(&stripeWords[12], &upper, sizeof(upper));
					dispatch->m_setStencilOperation(renderer, NULL, 2, 2, (LegoU32) row);
					dispatch->m_setStencilOperation(renderer, NULL, 1, 2, (LegoU32) row);
					dispatch->m_setStencilOperation(renderer, NULL, 0, 2, (LegoU32) row);
					dispatch->m_drawGuiRectangle(renderer, NULL, (RendererGuiRectangle*) stripeWords, defaults);
				}
				dispatch->m_setStencilOperation(renderer, NULL, 2, 0, 32);
				dispatch->m_setStencilOperation(renderer, NULL, 1, 0, 32);
				dispatch->m_setStencilOperation(renderer, NULL, 0, 0, 32);

				colorWords[10] = 0xffffffff;
				dispatch->m_applyTextureOperation(renderer, NULL, 0x0b);
				for (LegoS32 hue = 0x90; hue < 0x190; hue += 8) {
					LegoU32 color;
					BuildAnimationHueColor(&color, hue % 0x168, 0x3ff, 0x3ff);
					colorWords[10] = color;
					dispatch->m_drawGuiRectangle(renderer, NULL, (RendererGuiRectangle*) colorWords, defaults);
					dispatch->m_applyTextureOperation(renderer, NULL, 0x0c);
				}
				dispatch->m_applyTextureOperation(renderer, NULL, 0x0d);
				dispatch->m_setDepthState(renderer, NULL, 1);
			}
		}

		if (firstRenderableType3 < (LegoU32) g_gameObjectCount) {
			for (LegoS32 index = (LegoS32) firstRenderableType3; index < g_gameObjectCount; ++index) {
				GameObject* object = g_gameObjects[index];
				if ((object->m_type >> 8) != 3) {
					break;
				}
				if (*(LegoU32*) ((LegoU8*) object + 0x1c0) != 0 && (object->m_flags & 1) == 0) {
					PrepareGameObjectForRendering(object, NULL);
					if (*(LegoS32*) ((LegoU8*) object + 0x1c8) != -1) {
						RenderGlobalOverlayPass(&g_globalOverlayManager, NULL, 1);
					}
				}
			}
		}
	}

	EndProfileRange(renderingTimeRange);
	CharBuffer24 finishRenderingName("_Finish_Rendering");
	void* finishRenderingRange = BeginProfileRange(&finishRenderingName);
	if (beganScene == 1) {
		dispatch->m_endScene(renderer, NULL);
	}
	dispatch->m_present(renderer, NULL);
	if (dispatch->m_isSolidPassOverrideActive(renderer, NULL) == 0) {
		dispatch->m_setDeviceResourceMode(renderer, NULL, 7);
	}
	EndProfileRange(finishRenderingRange);
	FinishFrameAudioState();
}
