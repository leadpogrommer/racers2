#include "game/controller/gamecontrollerrendercallbacks.h"

#include "game/objects/gameobjectmodelbinding.h"
#include "game/state/gamerandom.h"
#include "math/matrix3.h"
#include "physics/dynamics/physicallifecycle.h"
#include "physics/timing/simulationtiming.h"
#include "render/buffers/rendererbufferdispatch.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/models/modelresource.h"
#include "render/resources/rendererrecords.h"

#include <math.h>
#include <string.h>

struct TrackTextureAnimation {
	GameObjectModelBinding* m_binding;
	PhysicalLifecycleState* m_lifecycle;
	LegoU32 m_fraction;
	LegoS32 m_resourceId;
};
DECOMP_SIZE_ASSERT(TrackTextureAnimation, 0x10);

// GLOBAL: LEGORACERS2 0x0054363c
static LegoFloat g_lightShaftAmplitude = 0.25f;
// GLOBAL: LEGORACERS2 0x00543640
static LegoFloat g_portalTextureScale = 1.0f;
// GLOBAL: LEGORACERS2 0x00570348
static LegoFloat g_lightShaftPhaseStep;
// GLOBAL: LEGORACERS2 0x00570350
static Vector3 g_slowCrystalTexturePosition;
// GLOBAL: LEGORACERS2 0x00570360
static Matrix3 g_portalTextureRotation;
// GLOBAL: LEGORACERS2 0x00570388
static Matrix3 g_slowCrystalTextureRotation;
// GLOBAL: LEGORACERS2 0x005703c0
static Vector3 g_crystalTexturePosition;
// GLOBAL: LEGORACERS2 0x005703ec
static LegoU8 g_slowCrystalInitialized;
// GLOBAL: LEGORACERS2 0x00570408
static Vector3 g_portalTextureAxisV;
// GLOBAL: LEGORACERS2 0x00570418
static Matrix3 g_crystalTextureRotation;
// GLOBAL: LEGORACERS2 0x00570468
static Matrix3 g_portalProjectionRotation;
// GLOBAL: LEGORACERS2 0x00570a80
static LegoU8 g_portalInitialized;
// GLOBAL: LEGORACERS2 0x00570a88
static Vector3 g_crystalTextureAngles;
// GLOBAL: LEGORACERS2 0x00570a98
static LegoFloat g_lightShaftColor[4];
// GLOBAL: LEGORACERS2 0x00570aac
static LegoU8 g_lightShaftInitialized;
// GLOBAL: LEGORACERS2 0x00570ab0
static Vector3 g_portalTexturePosition;
// GLOBAL: LEGORACERS2 0x00570ac0
static Vector3 g_slowCrystalTextureAngles;
// GLOBAL: LEGORACERS2 0x00570af8
static Vector3 g_portalTextureAngles;
// GLOBAL: LEGORACERS2 0x00570b08
static Vector3 g_portalTextureAxisU;
// GLOBAL: LEGORACERS2 0x00570b15
static LegoU8 g_crystalInitialized;
// GLOBAL: LEGORACERS2 0x00570b18
static TrackTextureAnimation g_trackTextureAnimations[16];
// GLOBAL: LEGORACERS2 0x00570c18
static LegoFloat g_crystalMaterialPhase;
// GLOBAL: LEGORACERS2 0x00570c1c
static LegoFloat g_slowCrystalMaterialPhase;
// GLOBAL: LEGORACERS2 0x00570c28
static LegoFloat g_lightShaftPhase;
// GLOBAL: LEGORACERS2 0x00570c2c
static LegoS32 g_trackTextureAnimationCount;
// GLOBAL: LEGORACERS2 0x00570c30
static LegoU32 g_trackTextureAnimationsDirty;
// GLOBAL: LEGORACERS2 0x00570c34
static LegoFloat g_portalScalePhase;
// GLOBAL: LEGORACERS2 0x00570c38
static LegoFloat g_portalOffsetPhaseU;
// GLOBAL: LEGORACERS2 0x00570c3c
static LegoFloat g_portalOffsetPhaseV;
// GLOBAL: LEGORACERS2 0x00570c40
static LegoFloat g_portalTextureOffsetU;
// GLOBAL: LEGORACERS2 0x00570c44
static LegoFloat g_portalTextureOffsetV;

static const LegoFloat g_materialPi = 3.1415927410125732f;

static void ApplyMaterialTextureTransform(const Matrix3* p_rotation, const Vector3* p_position)
{
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	dispatch->m_setTextureTransform(g_displayRenderer, NULL, 1, p_rotation, p_position);
	dispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	dispatch->m_setTextureTransform(g_displayRenderer, NULL, 9, p_rotation, p_position);
}

static void AdvanceCrystalMaterial(
	RendererAuxiliaryResourceRecord* p_material,
	LegoFloat* p_phase,
	LegoS32 p_period,
	LegoFloat p_highlightScale,
	LegoFloat p_highlightOffset
)
{
	LegoFloat baseColor, highlightColor;
	const LegoFloat baseScale = 0.15f, baseOffset = 0.45f;
	// The original keeps the phase and sine in x87 precision while storing the
	// phase for the next update. Both material colors use that same sine.
	// clang-format off
	__asm {
		mov eax, p_phase
		fld g_materialPi
		fadd st(0), st(0)
		fidiv p_period
		fadd dword ptr [eax]
		fst dword ptr [eax]
		fsin
		fld st(0)
		fmul baseScale
		fadd baseOffset
		fstp baseColor
		fmul p_highlightScale
		fadd p_highlightOffset
		fstp highlightColor
	}
	// clang-format on
	for (LegoS32 i = 0; i < 4; ++i)
	{
		p_material->m_color0[i] = baseColor;
		p_material->m_color1[i] = highlightColor;
	}
}

static LegoFloat NextMaterialRandom()
{
	GameFloatRandom* stream = &GetGameRandomState()->m_floatStreams[1];
	stream->m_frame = stream->m_frame * stream->m_multiplier + stream->m_increment;
	LegoU32 bits = (stream->m_frame & stream->m_mantissaMask) | stream->m_state;
	return *(LegoFloat*) &bits - 1.0f;
}

// STUB: LEGORACERS2 0x0045e090
LegoU32 __cdecl CrystalMaterialResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	if ((g_crystalInitialized & 2) == 0) {
		g_crystalTexturePosition.SetValues(0.0f, 0.0f, 0.0f);
	}
	if ((g_crystalInitialized & 4) == 0) {
		g_crystalTextureAngles.SetValues(0.0f, 0.0f, 0.0f);
	}
	// Matrix and vector exit callbacks are empty in the retail executable.
	g_crystalInitialized |= 7;
	if (p_phase == 0) {
		ApplyMaterialTextureTransform(&g_crystalTextureRotation, &g_crystalTexturePosition);
	}
	else if (p_phase == 1) {
		g_crystalTextureAngles.m_x += 0.0013f;
		g_crystalTextureAngles.m_y += 0.0025f;
		g_crystalTextureAngles.m_z += 0.004f;
		g_crystalTextureRotation.FUN_004a4f60(&g_crystalTextureAngles.m_x);
		AdvanceCrystalMaterial(p_material, &g_crystalMaterialPhase, (LegoS32) (g_simulationFrameRate * 8), 0.5f, 0.5f);
	}
	return 1;
}

// FUNCTION: LEGORACERS2 0x0045e2b0
LegoU32 __cdecl CrystalVariantMaterialResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	return CrystalMaterialResourceCallback(p_material, p_phase);
}

// STUB: LEGORACERS2 0x0045e2d0
LegoU32 __cdecl SlowCrystalMaterialResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	if ((g_slowCrystalInitialized & 2) == 0) {
		g_slowCrystalTexturePosition.SetValues(0.0f, 0.0f, 0.0f);
	}
	if ((g_slowCrystalInitialized & 4) == 0) {
		g_slowCrystalTextureAngles.SetValues(0.0f, 0.0f, 0.0f);
	}
	g_slowCrystalInitialized |= 7;
	if (p_phase == 0) {
		ApplyMaterialTextureTransform(&g_slowCrystalTextureRotation, &g_slowCrystalTexturePosition);
	}
	else if (p_phase == 1) {
		g_slowCrystalTextureAngles.m_x += 0.0013f;
		g_slowCrystalTextureAngles.m_y += 0.0025f;
		g_slowCrystalTextureAngles.m_z += 0.004f;
		g_slowCrystalTextureRotation.FUN_004a4f60(&g_slowCrystalTextureAngles.m_x);
		AdvanceCrystalMaterial(
			p_material,
			&g_slowCrystalMaterialPhase,
			(LegoS32) (g_simulationFrameRate << 4),
			0.3f,
			0.7f
		);
	}
	return 1;
}

// STUB: LEGORACERS2 0x0045e9e0
LegoU32 __cdecl LightShaftMaterialResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	if ((g_lightShaftInitialized & 1) == 0) {
		LegoFloat rate = (LegoFloat) (LegoS32) g_simulationFrameRate;
		g_lightShaftPhaseStep = (g_materialPi * 0.5f) / (rate + rate);
	}
	g_lightShaftInitialized |= 3;
	if (p_phase == 0) {
		for (LegoS32 i = 0; i < 4; ++i) {
			p_material->m_color0[i] = 0.0f;
			p_material->m_color1[i] = 0.0f;
			p_material->m_scaledColor1[i] = g_lightShaftColor[i];
		}
	}
	else if (p_phase == 1) {
		g_lightShaftPhase += g_lightShaftPhaseStep;
		if (g_lightShaftPhase > g_materialPi + g_materialPi) {
			g_lightShaftPhase -= g_materialPi + g_materialPi;
			if (NextMaterialRandom() > 0.8f) {
				g_lightShaftAmplitude = 0.4f;
			}
			else {
				g_lightShaftAmplitude = NextMaterialRandom() * 0.25f;
			}
		}
		g_lightShaftColor[0] = g_lightShaftColor[1] =
			(LegoFloat) (sin(g_lightShaftPhase) * g_lightShaftAmplitude + 0.5f);
		g_lightShaftColor[2] = g_lightShaftColor[3] = 0.0f;
	}
	return 1;
}

// STUB: LEGORACERS2 0x0045ec40
LegoU32 __cdecl TrackTextureResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32 p_phase)
{
	if (p_phase == 1) {
		g_trackTextureAnimationsDirty = 1;
	}
	else if (p_phase == 0) {
		GameObjectModelBinding* binding = (GameObjectModelBinding*) g_displayRenderer->m_unk0x4cc;
		if (g_trackTextureAnimationsDirty == 1) {
			for (LegoS32 i = 0; i < g_trackTextureAnimationCount; ++i) {
				TrackTextureAnimation* entry = &g_trackTextureAnimations[i];
				PhysicalLifecycleState* state = entry->m_lifecycle;
				Vector3 velocity = state->m_linearVelocity;
				LegoFloat square =
					velocity.m_z * velocity.m_z + velocity.m_y * velocity.m_y + velocity.m_x * velocity.m_x;
				LegoFloat speed;
				if (square > 0.001f) {
					const Matrix3* rotation = state->GetTransformDispatch()->m_rotation(state, NULL);
					double length = sqrt(square);
					double inverse = 1.0 / length;
					velocity.m_x = (LegoFloat) (velocity.m_x * inverse);
					velocity.m_y = (LegoFloat) (velocity.m_y * inverse);
					double z = velocity.m_z * inverse;
					speed = (LegoFloat) ((rotation->m_values[0][2] * velocity.m_x +
										  rotation->m_values[1][2] * velocity.m_y + rotation->m_values[2][2] * z) *
										 length * 0.02f);
					if (speed > 0.6f) {
						speed = 0.6f;
					}
					else if (speed < -0.6f) {
						speed = -0.6f;
					}
				}
				else {
					speed = 0.0f;
				}
				LegoFloat scaled = speed * 65536.0f;
				LegoS32 increment;
				// clang-format off
				__asm {
					fld scaled
					fistp increment
				}
				// clang-format on
				entry->m_fraction += increment;
			}
			g_trackTextureAnimationsDirty = 0;
		}
		for (LegoS32 i = 0; i < g_trackTextureAnimationCount; ++i) {
			TrackTextureAnimation* entry = &g_trackTextureAnimations[i];
			if (entry->m_binding == binding) {
				RendererResourceRef* reference = g_displayRenderer->FindTextureReferenceById(entry->m_resourceId, 1);
				if (reference != NULL) {
					typedef void(__fastcall * SetFraction)(RendererResourceCacheEntry*, void*, LegoU32);
					RendererResourceCacheEntry* resource = (RendererResourceCacheEntry*) reference->m_resource;
					((SetFraction*) resource->m_vtable)[5](resource, NULL, entry->m_fraction);
				}
				break;
			}
		}
	}
	return 1;
}

// FUNCTION: LEGORACERS2 0x0045edf0
void __fastcall RegisterTrackTextureAnimation(
	GameController*,
	void*,
	GameObjectModelBinding* p_binding,
	PhysicalLifecycleState* p_lifecycle,
	LegoS32 p_resourceId
)
{
	if (g_trackTextureAnimationCount != 16) {
		TrackTextureAnimation* entry = &g_trackTextureAnimations[g_trackTextureAnimationCount++];
		entry->m_binding = p_binding;
		entry->m_lifecycle = p_lifecycle;
		entry->m_resourceId = p_resourceId;
		entry->m_fraction = 0;
	}
}

// STUB: LEGORACERS2 0x0045ee30
void __fastcall UnregisterTrackTextureAnimation(GameController*, void*, GameObjectModelBinding* p_binding)
{
	LegoS32 count = g_trackTextureAnimationCount;
	for (LegoS32 i = 0; i < count; ++i) {
		if (g_trackTextureAnimations[i].m_binding == p_binding) {
			g_trackTextureAnimations[i--] = g_trackTextureAnimations[--count];
		}
	}
	g_trackTextureAnimationCount = count;
}

// STUB: LEGORACERS2 0x0045ee90
LegoU32 __cdecl PortalMaterialResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	if ((g_portalInitialized & 4) == 0) {
		g_portalTexturePosition.SetValues(0.0f, 0.0f, 0.0f);
	}
	if ((g_portalInitialized & 8) == 0) {
		g_portalTextureAngles.SetValues(0.0f, 0.0f, 0.0f);
	}
	g_portalInitialized |= 0x3f;
	if (p_phase == 0) {
		GameObjectModelBinding* binding = (GameObjectModelBinding*) g_displayRenderer->m_unk0x4cc;
		ModelResource* model = (ModelResource*) binding->m_model;
		for (LegoS32 groupIndex = 0; groupIndex < model->m_groupCount; ++groupIndex) {
			ModelTextureGroup* group = &model->m_groups[groupIndex];
			for (LegoS32 primitiveIndex = 0; primitiveIndex < group->m_primitiveCount; ++primitiveIndex) {
				ModelPrimitiveTextureRecord* primitive = &group->m_primitives[primitiveIndex];
				RendererAuxiliaryResourceRecord* material =
					g_displayRenderer->GetAuxiliaryResource(primitive->m_material);
				if (material == NULL || strcmp(material->m_path, "PORTAL") != 0) {
					continue;
				}
				primitive->m_flags |= 0x20;
				RendererVertexBuffer* buffer = primitive->m_vertexBuffer;
				const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) buffer->m_vtable;
				if (dispatch->m_lock(buffer, NULL, 0, 0, 0, 0) != TRUE) {
					continue;
				}
				dispatch->m_setCursor(buffer, NULL, 0);
				Vector3 minimum = {2147483648.0f, 2147483648.0f, 2147483648.0f};
				Vector3 maximum = {-2147483648.0f, -2147483648.0f, -2147483648.0f};
				LegoS32 count = (LegoS32) dispatch->m_getCount(buffer, NULL);
				for (LegoS32 vertex = 0; vertex < count; ++vertex) {
					Vector3* position = dispatch->m_getCurrentPosition(buffer, NULL);
					if (minimum.m_x > position->m_x) {
						minimum.m_x = position->m_x;
					}
					if (minimum.m_y > position->m_y) {
						minimum.m_y = position->m_y;
					}
					if (minimum.m_z > position->m_z) {
						minimum.m_z = position->m_z;
					}
					position = dispatch->m_getCurrentPosition(buffer, NULL);
					if (maximum.m_x < position->m_x) {
						maximum.m_x = position->m_x;
					}
					if (maximum.m_y < position->m_y) {
						maximum.m_y = position->m_y;
					}
					if (maximum.m_z < position->m_z) {
						maximum.m_z = position->m_z;
					}
					dispatch->m_advanceCursor(buffer, NULL);
				}
				LegoFloat inverseX = 1.0f / (maximum.m_x - minimum.m_x);
				LegoFloat inverseY = 1.0f / (maximum.m_y - minimum.m_y);
				dispatch->m_setCursor(buffer, NULL, 0);
				count = (LegoS32) dispatch->m_getCount(buffer, NULL);
				for (vertex = 0; vertex < count; ++vertex) {
					Vector3* position = dispatch->m_getCurrentPosition(buffer, NULL);
					LegoFloat x = (((position->m_x - minimum.m_x) * inverseX - 0.5f) + g_portalTextureOffsetU) *
								  g_portalTextureScale;
					position = dispatch->m_getCurrentPosition(buffer, NULL);
					double y = (((position->m_y - minimum.m_y) * inverseY - 0.5f) + g_portalTextureOffsetV) *
							   g_portalTextureScale;
					RendererTextureCoordinate coordinates;
					coordinates.m_u = (LegoFloat) (g_portalTextureAxisU.m_x * x + g_portalTextureAxisU.m_y * y + 0.5f);
					coordinates.m_v = (LegoFloat) (g_portalTextureAxisV.m_x * x + g_portalTextureAxisV.m_y * y + 0.5f);
					dispatch->m_setCurrentTextureCoordinate(buffer, NULL, 0, &coordinates);
					dispatch->m_advanceCursor(buffer, NULL);
				}
				dispatch->m_unlock(buffer, NULL, 1);
			}
		}
		ApplyMaterialTextureTransform(&g_portalTextureRotation, &g_portalTexturePosition);
	}
	else if (p_phase == 1) {
		g_portalTextureAngles.m_z += 0.004f;
		g_portalTextureRotation.FUN_004a4f60(&g_portalTextureAngles.m_x);
		Vector3 projectionAngles = {0.0f, 0.0f, g_portalTextureAngles.m_z * -3.0f};
		g_portalProjectionRotation.FUN_004a4f60(&projectionAngles.m_x);
		g_portalTextureAxisU.SetValues(
			g_portalProjectionRotation.m_values[0][0],
			g_portalProjectionRotation.m_values[1][0],
			g_portalProjectionRotation.m_values[2][0]
		);
		g_portalTextureAxisV.SetValues(
			g_portalProjectionRotation.m_values[0][1],
			g_portalProjectionRotation.m_values[1][1],
			g_portalProjectionRotation.m_values[2][1]
		);
		g_portalScalePhase += (g_materialPi + g_materialPi) / (LegoS32) (g_simulationFrameRate * 8);
		g_portalOffsetPhaseU += (g_materialPi + g_materialPi) / (LegoS32) (g_simulationFrameRate * 10);
		g_portalOffsetPhaseV += (g_materialPi + g_materialPi) / (LegoS32) (g_simulationFrameRate * 14);
		g_portalTextureScale = (LegoFloat) ((sin(g_portalScalePhase * 0.5f) * 0.5f + 0.5f) * 0.8f + 0.1f);
		g_portalTextureOffsetU = (LegoFloat) (sin(g_portalOffsetPhaseU) * 0.5f * (1.0f - g_portalTextureScale));
		g_portalTextureOffsetV = (LegoFloat) (sin(g_portalOffsetPhaseV) * 0.5f * (1.0f - g_portalTextureScale));
		p_material->m_modeFactor = (LegoFloat) (sin(g_portalScalePhase) * 0.2f + 0.4f);
	}
	else if (p_phase == 2) {
		for (LegoS32 i = 0; i < 4; ++i) {
			p_material->m_color0[i] = 0.0f;
			p_material->m_color1[i] = 0.0f;
			p_material->m_color2[i] = 0.0f;
			p_material->m_scaledColor1[i] = 0.7f;
		}
		p_material->m_modeFactor = 0.4f;
		p_material->m_mode = 1;
	}
	return 1;
}
