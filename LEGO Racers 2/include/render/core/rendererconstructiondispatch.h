#ifndef LR2_RENDERERCONSTRUCTIONDISPATCH_H
#define LR2_RENDERERCONSTRUCTIONDISPATCH_H

#include "decomp.h"
#include "render/buffers/rendererbuffer.h"
#include "render/buffers/rendererbufferobjects.h"
#include "render/core/rendererdispatch.h"
#include "render/draw/rendererdrawpacketview.h"
#include "render/draw/rendererquaddraw.h"
#include "render/resources/rendereranimationresource.h"
#include "render/resources/rendererrecords.h"
#include "render/terrain/rendererterrain.h"
#include "types.h"

#include <stddef.h>

class BitmapImage;
class DisplayRenderer;
class Matrix3;
class Quaternion;
class Terrain;
class Vector3;
struct RendererSpriteRecord;
struct RendererSpriteVertex;
struct RendererStagePlanRequest;

// These declarations model the observed callback ABI slot by slot. The
// fastcall bridges place the original ECX receiver in the first parameter and
// reserve EDX as an unused second parameter, leaving explicit arguments on the
// stack.
struct RendererMainDispatch {
	typedef void(__fastcall* Action)(DisplayRenderer*, void*);
	typedef LegoBool32(__fastcall* Predicate)(DisplayRenderer*, void*);
	typedef LegoU32(__fastcall* Value)(DisplayRenderer*, void*);
	typedef LegoS32(__fastcall* SignedValue)(DisplayRenderer*, void*);
	typedef LegoFloat(__fastcall* FloatValue)(DisplayRenderer*, void*);
	typedef void(__fastcall* SetFloat)(DisplayRenderer*, void*, LegoFloat);
	typedef void(__fastcall* SetU32)(DisplayRenderer*, void*, LegoU32);
	typedef void(__fastcall* SetS32)(DisplayRenderer*, void*, LegoS32);
	typedef void(__fastcall* SetBool)(DisplayRenderer*, void*, LegoBool32);
	typedef LegoBool32(__fastcall* PredicateU32)(DisplayRenderer*, void*, LegoU32);
	typedef LegoU32(__fastcall* ValueU32)(DisplayRenderer*, void*, LegoU32);
	typedef LegoS32(__fastcall* SignedValueU32)(DisplayRenderer*, void*, LegoU32);
	typedef void(__fastcall* SetFloat2)(DisplayRenderer*, void*, LegoFloat, LegoFloat);
	typedef void(__fastcall* Set2U32)(DisplayRenderer*, void*, LegoU32, LegoU32);
	typedef LegoBool32(__fastcall* Predicate3U32)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32);
	typedef LegoBool32(__fastcall* Predicate6U32)(
		DisplayRenderer*,
		void*,
		LegoU32,
		LegoU32,
		LegoU32,
		LegoU32,
		LegoU32,
		LegoU32
	);
	typedef void(__fastcall* Set3U32)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* Set4U32)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* Set5U32)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* SetProjection)(
		DisplayRenderer*,
		void*,
		LegoFloat,
		LegoFloat,
		LegoFloat,
		LegoFloat,
		LegoFloat,
		LegoU32
	);
	typedef void(__fastcall* SetRotationTranslation)(DisplayRenderer*, void*, const Matrix3*, const Vector3*);
	typedef void(__fastcall* SetQuaternionTranslation)(DisplayRenderer*, void*, const Quaternion*, const Vector3*);
	typedef void(__fastcall* SelectLights)(DisplayRenderer*, void*, const Vector3*, LegoFloat, LegoU32, LegoU32*);
	typedef RendererVertexBuffer*(__fastcall*
									  CreateVertexBuffer)(DisplayRenderer*, void*, LegoU32, LegoS32, LegoS32, LegoU32);
	typedef void(__fastcall* SetPointer)(DisplayRenderer*, void*, const void*);
	typedef void(__fastcall* SetStageRequest)(DisplayRenderer*, void*, RendererStagePlanRequest*);
	typedef void(__fastcall* SetDrawPacket)(DisplayRenderer*, void*, RendererDrawPacketView*);
	typedef void(__fastcall* SetTextureRecord)(DisplayRenderer*, void*, RendererTextureRecord*);
	typedef void(__fastcall*
					 SetGuiRectangle)(DisplayRenderer*, void*, const RendererGuiRectangle*, RendererDefaultRecord*);
	typedef void(__fastcall* SetPointer2)(DisplayRenderer*, void*, const void*, const void*);
	typedef void(__fastcall* SetIdPointer)(DisplayRenderer*, void*, LegoS32, const void*, const void*);
	typedef LegoBool32(__fastcall* PredicatePointer2)(DisplayRenderer*, void*, const Vector3*, Vector3*);
	typedef LegoFloat(__fastcall* TransformPoint)(DisplayRenderer*, void*, const Vector3*, Vector3*);
	typedef LegoFloat(__fastcall* ScaleCoordinate)(DisplayRenderer*, void*, LegoFloat, LegoFloat);
	typedef LegoFloat*(__fastcall* FloatPointer)(DisplayRenderer*, void*);
	typedef RendererVertexBuffer*(__fastcall* VertexBufferPointer)(DisplayRenderer*, void*);
	typedef RendererTextureRecord*(__fastcall* TextureRecordIndex)(DisplayRenderer*, void*, LegoU32);
	typedef RendererAnimationResourceRecord*(__fastcall* AnimationResource)(
		DisplayRenderer*,
		void*,
		const LegoChar*,
		LegoBool32
	);
	typedef RendererIndexBuffer*(__fastcall* CreateIndexBuffer)(DisplayRenderer*, void*, LegoU32, LegoS32);
	typedef RendererTextureRecord*(__fastcall* FindTextureRecord)(
		DisplayRenderer*,
		void*,
		const LegoChar*,
		LegoU32,
		LegoS32,
		LegoU32,
		LegoU32,
		LegoBool32
	);
	typedef LegoBool32(__fastcall* CreateTextureRecord)(
		DisplayRenderer*,
		void*,
		LegoU32,
		LegoS32,
		LegoU32,
		LegoU32,
		LegoU32,
		LegoU32*,
		LegoU32
	);
	typedef LegoBool32(__fastcall* BuildMipSidecar)(
		DisplayRenderer*,
		void*,
		const LegoChar*,
		const LegoChar*,
		BitmapImage*
	);
	typedef LegoBool32(__fastcall* LoadTextureImage)(
		DisplayRenderer*,
		void*,
		LegoS32,
		BitmapImage*,
		LegoS32,
		const LegoChar*
	);
	typedef LegoBool32(__fastcall* LoadCubeFace)(
		DisplayRenderer*,
		void*,
		LegoS32,
		BitmapImage*,
		const LegoChar*,
		LegoS32
	);
	typedef void(__fastcall* SetTextureTransform)(DisplayRenderer*, void*, LegoS32, const Matrix3*, const Vector3*);
	typedef LegoU32(__fastcall* UpdateTextureStageFlags)(DisplayRenderer*, void*, RendererDefaultSlot*, LegoU32);
	typedef void(__fastcall* DrawTerrain)(DisplayRenderer*, void*, Terrain*);
	typedef void(__stdcall* TerrainGeometry)(
		Terrain*,
		Vector3*,
		const Vector3*,
		LegoS32,
		RendererTerrainGeometryPacket*,
		LegoS32
	);
	typedef LegoS32(__fastcall* AddViewport)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* UpdateViewport)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* SetTextureIndexMap)(DisplayRenderer*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* GetMetrics)(DisplayRenderer*, void*, LegoFloat*);
	typedef void(__fastcall* DrawLines)(DisplayRenderer*, void*, const Vector3*, LegoS32);
	typedef void(__fastcall* DrawTriangles)(DisplayRenderer*, void*, const void*, LegoS32);
	typedef void(__fastcall* SpriteBatch)(DisplayRenderer*, void*, LegoS32, LegoU32, LegoFloat, LegoFloat);
	typedef void(__fastcall* DrawSprite)(DisplayRenderer*, void*, RendererSpriteRecord*);
	typedef RendererSpriteVertex*(__fastcall* AllocateSpriteVertices)(DisplayRenderer*, void*);
	typedef void(__stdcall* TransformWorldView)(const Vector3*, Vector3*);
	typedef void(__stdcall* Diagnostic)(const LegoChar*);
	typedef LegoBool32(__stdcall* GenerateNormalMap)(const LegoChar*, BitmapImage*);
	typedef LegoBool32(__fastcall* DisplayMode)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoS32);
	typedef LegoU32(__fastcall* GetModeCount)(DisplayRenderer*, void*);
	typedef void(__fastcall* GetMode)(DisplayRenderer*, void*, LegoU32, LegoU32*, LegoU32*, LegoU32*, LegoU32*);
	typedef int(__cdecl* Purecall)(void);

	Predicate m_initialize;                                                   // 0x000
	Action m_shutdown;                                                        // 0x004
	PredicateU32 m_setDeviceResourceMode;                                     // 0x008
	Predicate m_beginScene;                                                   // 0x00c
	Action m_endScene;                                                        // 0x010
	Action m_noOp014;                                                         // 0x014
	Action m_noOp018;                                                         // 0x018
	SetU32 m_setMaterialColor;                                                // 0x01c
	PredicateU32 m_clearBuffers;                                              // 0x020
	Predicate m_present;                                                      // 0x024
	SetProjection m_setProjection;                                            // 0x028
	SetQuaternionTranslation m_setWorldQuaternion;                            // 0x02c
	SetRotationTranslation m_setWorldMatrix;                                  // 0x030
	Action m_applyDefaultWorldTransform;                                      // 0x034
	SetRotationTranslation m_setViewMatrix;                                   // 0x038
	SelectLights m_selectLights;                                              // 0x03c
	FloatPointer m_getSelectedLightDirection;                                 // 0x040
	CreateVertexBuffer m_createVertexBuffer;                                  // 0x044
	SetStageRequest m_prepareDrawPlan;                                        // 0x048
	SetDrawPacket m_drawRendererPacket;                                       // 0x04c
	SetDrawPacket m_drawIndexedPacket;                                        // 0x050
	PredicatePointer2 m_isPointWithinViewport;                                // 0x054
	ScaleCoordinate m_scaleProjectionCoordinate;                              // 0x058
	TransformWorldView m_transformStaticWorldView;                            // 0x05c
	TransformPoint m_transformProjectedPoint;                                 // 0x060
	TransformPoint m_transformViewportPoint;                                  // 0x064
	Action m_updateTransformCache;                                            // 0x068
	PredicateU32 m_shouldUseTextureResourceIndex;                             // 0x06c
	PredicateU32 m_isTextureResourceKindAccepted;                             // 0x070
	PredicateU32 m_isTextureResourceKindSpecial;                              // 0x074
	PredicateU32 m_acceptUnusedRendererPolicy;                                // 0x078
	Predicate m_hasPreferredDepthFormat;                                      // 0x07c
	SetStageRequest m_resolveTextureStagePlan;                                // 0x080
	SetStageRequest m_buildBumpMapVectors;                                    // 0x084
	SetU32 m_setStencilBufferEnabled;                                         // 0x088
	SetU32 m_resetStencilOperations;                                          // 0x08c
	SetU32 m_setStencilFunction;                                              // 0x090
	Set3U32 m_setStencilOperation;                                            // 0x094
	SetU32 m_setDepthState;                                                   // 0x098
	SetU32 m_setAlphaTestReference;                                           // 0x09c
	Action m_setDepthAlwaysWrite;                                             // 0x0a0
	SetU32 m_adjustSolidPassScope;                                            // 0x0a4
	Predicate m_isSolidPassOverrideActive;                                    // 0x0a8
	Predicate3U32 m_rejectUnknownDrawRequest;                                 // 0x0ac
	AddViewport m_addViewport;                                                // 0x0b0
	UpdateViewport m_updateViewport;                                          // 0x0b4
	SetU32 m_selectViewport;                                                  // 0x0b8
	SetU32 m_releaseViewport;                                                 // 0x0bc
	SetU32 m_setColorOverride;                                                // 0x0c0
	Action m_disableColorOverride;                                            // 0x0c4
	SetU32 m_setAlphaBlendMode;                                               // 0x0c8
	SetU32 m_setBlendModeOverride;                                            // 0x0cc
	Action m_clearBlendModeOverride;                                          // 0x0d0
	CreateTextureRecord m_createTextureRecord;                                // 0x0d4
	BuildMipSidecar m_buildMipSidecar;                                        // 0x0d8
	GenerateNormalMap m_generateNormalMap;                                    // 0x0dc
	LoadTextureImage m_loadTextureImage;                                      // 0x0e0
	SetS32 m_releaseTextureRecord;                                            // 0x0e4
	Predicate m_supportsCubeTextures;                                         // 0x0e8
	LoadCubeFace m_loadCubeTextureFace;                                       // 0x0ec
	SetTextureRecord m_setTextureRenderTarget;                                // 0x0f0
	SetGuiRectangle m_drawGuiRectangle;                                       // 0x0f4
	DrawTerrain m_drawTerrain;                                                // 0x0f8
	TerrainGeometry m_buildTerrainGeometry;                                   // 0x0fc
	CreateIndexBuffer m_createIndexBuffer;                                    // 0x100
	SetU32 m_setFogEnabled;                                                   // 0x104
	SetU32 m_setFogColor;                                                     // 0x108
	SetFloat2 m_setFogRange;                                                  // 0x10c
	ValueU32 m_getDotProductStageFlag;                                        // 0x110
	TextureRecordIndex m_getTextureRecord;                                    // 0x114
	AnimationResource m_findOrAllocateAnimationResource;                      // 0x118
	SetTextureIndexMap m_setTextureIndexMap;                                  // 0x11c
	UpdateTextureStageFlags m_updateTextureStageFlags;                        // 0x120
	SetU32 m_applyTextureOperation;                                           // 0x124
	FloatValue m_getStageBlend;                                               // 0x128
	SetFloat m_setStageBlend;                                                 // 0x12c
	GetMetrics m_getTimingMetrics;                                            // 0x130
	Value m_getMaximumTextureDimension;                                       // 0x134
	DrawLines m_drawUnlitLines;                                               // 0x138
	DrawTriangles m_drawUnlitTriangles;                                       // 0x13c
	SetPointer m_drawQuadPacket;                                              // 0x140
	SpriteBatch m_beginSpriteBatch;                                           // 0x144
	DrawSprite m_drawSpriteGeometry;                                          // 0x148
	DrawSprite m_drawSprite;                                                  // 0x14c
	void(__fastcall* m_drawStencilVolumes)(DisplayRenderer*, void*, LegoS32); // 0x150
	Action m_drawStencilOverlay;                                              // 0x154
	VertexBufferPointer m_getQuadVertexBuffer;                                // 0x158
	Predicate6U32 m_supportsUnknownDrawFeature;                               // 0x15c
	Diagnostic m_diagnostic;                                                  // 0x160
	Predicate3U32 m_rejectUnknownDrawRequestAgain;                            // 0x164
	SetTextureTransform m_setTextureTransform;                                // 0x168
	GetModeCount m_getSelectedAdapterModeCount;                               // 0x16c
	GetMode m_getSelectedAdapterMode;                                         // 0x170
	DisplayMode m_reconfigureDisplayMode;                                     // 0x174
	Predicate m_shouldUseHighResolutionTextures;                              // 0x178
	SetBool m_setClockwiseCulling;                                            // 0x17c
	SetU32 m_setUnknownDrawPolicy;                                            // 0x180
	SignedValue m_getTextureQuality;                                          // 0x184
	SetU32 m_setTextureQuality;                                               // 0x188
	Predicate m_advanceDeviceFallback;                                        // 0x18c
	Predicate m_supportsExtendedTextureStages;                                // 0x190
	SignedValue m_getDeviceMemoryMegabytes;                                   // 0x194
	Predicate m_usesHardwareVertexProcessing;                                 // 0x198
};

DECOMP_SIZE_ASSERT(RendererMainDispatch, 0x19c);
DECOMP_STATIC_ASSERT(offsetof(RendererMainDispatch, m_drawTerrain) == 0xf8);
DECOMP_STATIC_ASSERT(offsetof(RendererMainDispatch, m_buildTerrainGeometry) == 0xfc);
DECOMP_STATIC_ASSERT(offsetof(RendererMainDispatch, m_drawQuadPacket) == 0x140);

// The constructor first installs the observed 103-entry base table, then
// replaces it with the derived 103-entry table after initializing members.
struct RendererBaseDispatch {
#define LR2_RENDERER_BASE_FIELDS                                                                                       \
	RendererMainDispatch::Predicate m_initialize;                                                                      \
	RendererMainDispatch::Action m_shutdown;                                                                           \
	RendererMainDispatch::PredicateU32 m_setDeviceResourceMode;                                                        \
	RendererMainDispatch::Predicate m_beginScene;                                                                      \
	RendererMainDispatch::Action m_endScene;                                                                           \
	RendererMainDispatch::Action m_noOp014;                                                                            \
	RendererMainDispatch::Action m_noOp018;                                                                            \
	RendererMainDispatch::SetU32 m_setMaterialColor;                                                                   \
	RendererMainDispatch::PredicateU32 m_clearBuffers;                                                                 \
	RendererMainDispatch::Predicate m_present;                                                                         \
	RendererMainDispatch::SetProjection m_setProjection;                                                               \
	RendererMainDispatch::SetQuaternionTranslation m_setWorldQuaternion;                                               \
	RendererMainDispatch::SetRotationTranslation m_setWorldMatrix;                                                     \
	RendererMainDispatch::Action m_applyDefaultWorldTransform;                                                         \
	RendererMainDispatch::SetRotationTranslation m_setViewMatrix;                                                      \
	RendererMainDispatch::SelectLights m_selectLights;                                                                 \
	RendererMainDispatch::FloatPointer m_getSelectedLightDirection;                                                    \
	RendererMainDispatch::CreateVertexBuffer m_createVertexBuffer;                                                     \
	RendererMainDispatch::SetStageRequest m_prepareDrawPlan;                                                           \
	RendererMainDispatch::SetDrawPacket m_drawRendererPacket;                                                          \
	RendererMainDispatch::SetDrawPacket m_drawIndexedPacket;                                                           \
	RendererMainDispatch::PredicatePointer2 m_isPointWithinViewport;                                                   \
	RendererMainDispatch::ScaleCoordinate m_scaleProjectionCoordinate;                                                 \
	RendererMainDispatch::TransformWorldView m_transformStaticWorldView;                                               \
	RendererMainDispatch::TransformPoint m_transformProjectedPoint;                                                    \
	RendererMainDispatch::TransformPoint m_transformViewportPoint;                                                     \
	RendererMainDispatch::Action m_updateTransformCache;                                                               \
	RendererMainDispatch::PredicateU32 m_shouldUseTextureResourceIndex;                                                \
	RendererMainDispatch::PredicateU32 m_isTextureResourceKindAccepted;                                                \
	RendererMainDispatch::PredicateU32 m_isTextureResourceKindSpecial;                                                 \
	RendererMainDispatch::PredicateU32 m_acceptUnusedRendererPolicy;                                                   \
	RendererMainDispatch::Predicate m_hasPreferredDepthFormat;                                                         \
	RendererMainDispatch::SetStageRequest m_resolveTextureStagePlan;                                                   \
	RendererMainDispatch::SetStageRequest m_buildBumpMapVectors;                                                       \
	RendererMainDispatch::SetU32 m_setStencilBufferEnabled;                                                            \
	RendererMainDispatch::SetU32 m_resetStencilOperations;                                                             \
	RendererMainDispatch::SetU32 m_setStencilFunction;                                                                 \
	RendererMainDispatch::Set3U32 m_setStencilOperation;                                                               \
	RendererMainDispatch::SetU32 m_setDepthState;                                                                      \
	RendererMainDispatch::SetU32 m_setAlphaTestReference;                                                              \
	RendererMainDispatch::Action m_setDepthAlwaysWrite;                                                                \
	RendererMainDispatch::SetU32 m_adjustSolidPassScope;                                                               \
	RendererMainDispatch::Predicate m_isSolidPassOverrideActive;                                                       \
	RendererMainDispatch::Predicate3U32 m_rejectUnknownDrawRequest;                                                    \
	RendererMainDispatch::AddViewport m_addViewport;                                                                   \
	RendererMainDispatch::UpdateViewport m_updateViewport;                                                             \
	RendererMainDispatch::SetU32 m_selectViewport;                                                                     \
	RendererMainDispatch::SetU32 m_releaseViewport;                                                                    \
	RendererMainDispatch::SetU32 m_setColorOverride;                                                                   \
	RendererMainDispatch::Action m_disableColorOverride;                                                               \
	RendererMainDispatch::SetU32 m_setAlphaBlendMode;                                                                  \
	RendererMainDispatch::SetU32 m_setBlendModeOverride;                                                               \
	RendererMainDispatch::Action m_clearBlendModeOverride;                                                             \
	RendererMainDispatch::CreateTextureRecord m_createTextureRecord;                                                   \
	RendererMainDispatch::BuildMipSidecar m_buildMipSidecar;                                                           \
	RendererMainDispatch::GenerateNormalMap m_generateNormalMap;                                                       \
	RendererMainDispatch::LoadTextureImage m_loadTextureImage;                                                         \
	RendererMainDispatch::SetS32 m_releaseTextureRecord;                                                               \
	RendererMainDispatch::Predicate m_supportsCubeTextures;                                                            \
	RendererMainDispatch::LoadCubeFace m_loadCubeTextureFace;                                                          \
	RendererMainDispatch::SetTextureRecord m_setTextureRenderTarget;                                                   \
	RendererMainDispatch::SetGuiRectangle m_drawGuiRectangle;                                                          \
	RendererMainDispatch::DrawTerrain m_drawTerrain;                                                                   \
	RendererMainDispatch::TerrainGeometry m_buildTerrainGeometry;                                                      \
	RendererMainDispatch::CreateIndexBuffer m_createIndexBuffer;                                                       \
	RendererMainDispatch::SetU32 m_setFogEnabled;                                                                      \
	RendererMainDispatch::SetU32 m_setFogColor;                                                                        \
	RendererMainDispatch::SetFloat2 m_setFogRange;                                                                     \
	RendererMainDispatch::ValueU32 m_getDotProductStageFlag;                                                           \
	RendererMainDispatch::TextureRecordIndex m_getTextureRecord;                                                       \
	RendererMainDispatch::AnimationResource m_findOrAllocateAnimationResource;                                         \
	RendererMainDispatch::SetTextureIndexMap m_setTextureIndexMap;                                                     \
	RendererMainDispatch::UpdateTextureStageFlags m_updateTextureStageFlags;                                           \
	RendererMainDispatch::SetU32 m_applyTextureOperation;                                                              \
	RendererMainDispatch::FloatValue m_getStageBlend;                                                                  \
	void(__fastcall * m_setStageBlend)(DisplayRenderer*, void*, LegoFloat);                                            \
	RendererMainDispatch::GetMetrics m_getTimingMetrics;                                                               \
	RendererMainDispatch::Value m_getMaximumTextureDimension;                                                          \
	RendererMainDispatch::DrawLines m_drawUnlitLines;                                                                  \
	RendererMainDispatch::DrawTriangles m_drawUnlitTriangles;                                                          \
	RendererMainDispatch::SetPointer m_drawQuadPacket;                                                                 \
	RendererMainDispatch::SpriteBatch m_beginSpriteBatch;                                                              \
	RendererMainDispatch::DrawSprite m_drawSpriteGeometry;                                                             \
	RendererMainDispatch::DrawSprite m_drawSprite;                                                                     \
	RendererMainDispatch::SetS32 m_drawStencilVolumes;                                                                 \
	RendererMainDispatch::Action m_drawStencilOverlay;                                                                 \
	RendererMainDispatch::VertexBufferPointer m_getQuadVertexBuffer;                                                   \
	RendererMainDispatch::Predicate6U32 m_supportsUnknownDrawFeature;                                                  \
	RendererMainDispatch::Diagnostic m_diagnostic;                                                                     \
	RendererMainDispatch::Predicate3U32 m_rejectUnknownDrawRequestAgain;                                               \
	RendererMainDispatch::SetTextureTransform m_setTextureTransform;                                                   \
	RendererMainDispatch::GetModeCount m_getSelectedAdapterModeCount;                                                  \
	RendererMainDispatch::GetMode m_getSelectedAdapterMode;                                                            \
	RendererMainDispatch::DisplayMode m_reconfigureDisplayMode;                                                        \
	RendererMainDispatch::Predicate m_shouldUseHighResolutionTextures;                                                 \
	RendererMainDispatch::SetBool m_setClockwiseCulling;                                                               \
	RendererMainDispatch::SetU32 m_setUnknownDrawPolicy;                                                               \
	RendererMainDispatch::SignedValue m_getTextureQuality;                                                             \
	RendererMainDispatch::SetU32 m_setTextureQuality;                                                                  \
	RendererMainDispatch::Predicate m_advanceDeviceFallback;                                                           \
	RendererMainDispatch::Predicate m_supportsExtendedTextureStages;                                                   \
	RendererMainDispatch::SignedValue m_getDeviceMemoryMegabytes;                                                      \
	RendererMainDispatch::Predicate m_usesHardwareVertexProcessing
	LR2_RENDERER_BASE_FIELDS;
#undef LR2_RENDERER_BASE_FIELDS
};

DECOMP_SIZE_ASSERT(RendererBaseDispatch, 0x19c);
extern const RendererBaseDispatch g_rendererBaseDispatch;
extern const RendererMainDispatch g_rendererDerivedDispatch;

struct RendererLookupDispatch {
	typedef void(__fastcall* Insert)(RendererLookupTableState*, void*, LegoU32, LegoU32, LegoU32, LegoS32);
	typedef void(__fastcall* Flush)(RendererLookupTableState*, void*);
	Insert m_insert;
	Flush m_flush;
};

DECOMP_SIZE_ASSERT(RendererLookupDispatch, 8);
extern const RendererLookupDispatch g_rendererBucketedLookupDispatch;
extern const RendererLookupDispatch g_rendererOrderedLookupDispatch;
extern const RendererLookupDispatch g_rendererDeferredLookupDispatch;

#endif // LR2_RENDERERCONSTRUCTIONDISPATCH_H
