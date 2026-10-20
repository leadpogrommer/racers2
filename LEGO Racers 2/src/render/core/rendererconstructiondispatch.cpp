#include "render/core/rendererconstructiondispatch.h"

#include "core/application/applicationpolicy.h"
#include "render/core/renderer.h"
#include "render/terrain/rendererterrain.h"

extern "C" int __cdecl _purecall(void);
extern LegoU32 ApplicationTruePolicy();
void ApplicationNoOp();
void __fastcall ApplyDefaultWorldTransform(DisplayRenderer*, void*);
void __fastcall ResolveRendererTextureStagePlan(DisplayRenderer*, void*, RendererStagePlanRequest*);
void __stdcall TransformStaticWorldViewPoint(const Vector3*, Vector3*);
LegoBool32 __stdcall GenerateNormalMap(const LegoChar*, BitmapImage*);

#define LR2_BRIDGE0(ReturnType, BridgeName, MethodName)                                                                \
	ReturnType __fastcall BridgeName(DisplayRenderer* p_renderer, void*)                                               \
	{                                                                                                                  \
		return p_renderer->MethodName();                                                                               \
	}

#define LR2_BRIDGE1(ReturnType, BridgeName, MethodName, Type1)                                                         \
	ReturnType __fastcall BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1)                                 \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1);                                                                         \
	}

#define LR2_BRIDGE2(ReturnType, BridgeName, MethodName, Type1, Type2)                                                  \
	ReturnType __fastcall BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1, Type2 p_arg2)                   \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1, p_arg2);                                                                 \
	}

#define LR2_BRIDGE3(ReturnType, BridgeName, MethodName, Type1, Type2, Type3)                                           \
	ReturnType __fastcall BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1, Type2 p_arg2, Type3 p_arg3)     \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1, p_arg2, p_arg3);                                                         \
	}

#define LR2_BRIDGE7(ReturnType, BridgeName, MethodName, Type1, Type2, Type3, Type4, Type5, Type6, Type7)               \
	ReturnType __fastcall BridgeName(                                                                                  \
		DisplayRenderer* p_renderer,                                                                                   \
		void*,                                                                                                         \
		Type1 p_arg1,                                                                                                  \
		Type2 p_arg2,                                                                                                  \
		Type3 p_arg3,                                                                                                  \
		Type4 p_arg4,                                                                                                  \
		Type5 p_arg5,                                                                                                  \
		Type6 p_arg6,                                                                                                  \
		Type7 p_arg7                                                                                                   \
	)                                                                                                                  \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4, p_arg5, p_arg6, p_arg7);                         \
	}

#define LR2_BRIDGE0_VOID(BridgeName, MethodName)                                                                       \
	void __fastcall BridgeName(DisplayRenderer* p_renderer, void*)                                                     \
	{                                                                                                                  \
		p_renderer->MethodName();                                                                                      \
	}
#define LR2_BRIDGE1_VOID(BridgeName, MethodName, Type1)                                                                \
	void __fastcall BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1)                                       \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1);                                                                                \
	}
#define LR2_BRIDGE2_VOID(BridgeName, MethodName, Type1, Type2)                                                         \
	void __fastcall BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1, Type2 p_arg2)                         \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1, p_arg2);                                                                        \
	}
#define LR2_BRIDGE3_VOID(BridgeName, MethodName, Type1, Type2, Type3)                                                  \
	void __fastcall BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1, Type2 p_arg2, Type3 p_arg3)           \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1, p_arg2, p_arg3);                                                                \
	}
#define LR2_BRIDGE4_VOID(BridgeName, MethodName, Type1, Type2, Type3, Type4)                                           \
	void __fastcall                                                                                                    \
	BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1, Type2 p_arg2, Type3 p_arg3, Type4 p_arg4)             \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4);                                                        \
	}
#define LR2_BRIDGE5_VOID(BridgeName, MethodName, Type1, Type2, Type3, Type4, Type5)                                    \
	void __fastcall BridgeName(                                                                                        \
		DisplayRenderer* p_renderer,                                                                                   \
		void*,                                                                                                         \
		Type1 p_arg1,                                                                                                  \
		Type2 p_arg2,                                                                                                  \
		Type3 p_arg3,                                                                                                  \
		Type4 p_arg4,                                                                                                  \
		Type5 p_arg5                                                                                                   \
	)                                                                                                                  \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4, p_arg5);                                                \
	}
#define LR2_BRIDGE6_VOID(BridgeName, MethodName, Type1, Type2, Type3, Type4, Type5, Type6)                             \
	void __fastcall BridgeName(                                                                                        \
		DisplayRenderer* p_renderer,                                                                                   \
		void*,                                                                                                         \
		Type1 p_arg1,                                                                                                  \
		Type2 p_arg2,                                                                                                  \
		Type3 p_arg3,                                                                                                  \
		Type4 p_arg4,                                                                                                  \
		Type5 p_arg5,                                                                                                  \
		Type6 p_arg6                                                                                                   \
	)                                                                                                                  \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4, p_arg5, p_arg6);                                        \
	}
#define LR2_BRIDGE7_VOID(BridgeName, MethodName, Type1, Type2, Type3, Type4, Type5, Type6, Type7)                      \
	void __fastcall BridgeName(                                                                                        \
		DisplayRenderer* p_renderer,                                                                                   \
		void*,                                                                                                         \
		Type1 p_arg1,                                                                                                  \
		Type2 p_arg2,                                                                                                  \
		Type3 p_arg3,                                                                                                  \
		Type4 p_arg4,                                                                                                  \
		Type5 p_arg5,                                                                                                  \
		Type6 p_arg6,                                                                                                  \
		Type7 p_arg7                                                                                                   \
	)                                                                                                                  \
	{                                                                                                                  \
		p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4, p_arg5, p_arg6, p_arg7);                                \
	}

#define LR2_BRIDGE4(ReturnType, BridgeName, MethodName, Type1, Type2, Type3, Type4)                                    \
	ReturnType __fastcall                                                                                              \
	BridgeName(DisplayRenderer* p_renderer, void*, Type1 p_arg1, Type2 p_arg2, Type3 p_arg3, Type4 p_arg4)             \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4);                                                 \
	}

#define LR2_BRIDGE5(ReturnType, BridgeName, MethodName, Type1, Type2, Type3, Type4, Type5)                             \
	ReturnType __fastcall BridgeName(                                                                                  \
		DisplayRenderer* p_renderer,                                                                                   \
		void*,                                                                                                         \
		Type1 p_arg1,                                                                                                  \
		Type2 p_arg2,                                                                                                  \
		Type3 p_arg3,                                                                                                  \
		Type4 p_arg4,                                                                                                  \
		Type5 p_arg5                                                                                                   \
	)                                                                                                                  \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4, p_arg5);                                         \
	}

#define LR2_BRIDGE6(ReturnType, BridgeName, MethodName, Type1, Type2, Type3, Type4, Type5, Type6)                      \
	ReturnType __fastcall BridgeName(                                                                                  \
		DisplayRenderer* p_renderer,                                                                                   \
		void*,                                                                                                         \
		Type1 p_arg1,                                                                                                  \
		Type2 p_arg2,                                                                                                  \
		Type3 p_arg3,                                                                                                  \
		Type4 p_arg4,                                                                                                  \
		Type5 p_arg5,                                                                                                  \
		Type6 p_arg6                                                                                                   \
	)                                                                                                                  \
	{                                                                                                                  \
		return p_renderer->MethodName(p_arg1, p_arg2, p_arg3, p_arg4, p_arg5, p_arg6);                                 \
	}

LR2_BRIDGE0(LegoBool32, RendererDispatchInitialize, Initialize)
LR2_BRIDGE0_VOID(RendererDispatchShutdown, Shutdown)
LR2_BRIDGE1(LegoBool32, RendererDispatchSetDeviceResourceMode, SetDeviceResourceMode, LegoU32)
LR2_BRIDGE0(LegoBool32, RendererDispatchBeginScene, BeginScene)
LR2_BRIDGE0_VOID(RendererDispatchEndScene, EndScene)

void __fastcall RendererDispatchNoOp(DisplayRenderer*, void*)
{
	ApplicationNoOp();
}

LR2_BRIDGE1_VOID(RendererDispatchSetMaterialColor, SetMaterialColor, LegoU32)
LR2_BRIDGE1(LegoBool32, RendererDispatchClearBuffers, ClearBuffers, LegoU32)
LR2_BRIDGE0(LegoBool32, RendererDispatchPresent, Present)
LR2_BRIDGE6_VOID(
	RendererDispatchSetProjection,
	SetProjection,
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoU32
)
LR2_BRIDGE2_VOID(RendererDispatchSetWorldQuaternion, SetWorldTransform, const Quaternion*, const Vector3*)
LR2_BRIDGE2_VOID(RendererDispatchSetWorldMatrix, SetWorldTransform, const Matrix3*, const Vector3*)
LR2_BRIDGE2_VOID(RendererDispatchSetViewMatrix, SetViewTransform, const Matrix3*, const Vector3*)
LR2_BRIDGE4_VOID(RendererDispatchSelectLights, SelectLights, const Vector3*, LegoFloat, LegoU32, LegoU32*)
LR2_BRIDGE0(LegoFloat*, RendererDispatchGetSelectedLightDirection, GetSelectedLightDirection)
LR2_BRIDGE4(
	RendererVertexBuffer*,
	RendererDispatchCreateVertexBuffer,
	CreateVertexBuffer,
	LegoU32,
	LegoS32,
	LegoS32,
	LegoU32
)
LR2_BRIDGE1_VOID(RendererDispatchPrepareDrawPlan, PrepareDrawPlan, RendererStagePlanRequest*)
LR2_BRIDGE1_VOID(RendererDispatchDrawRendererPacket, DrawRendererPacket, RendererDrawPacketView*)
LR2_BRIDGE1_VOID(RendererDispatchDrawIndexedPacket, DrawIndexedPacket, RendererDrawPacketView*)
LR2_BRIDGE2(LegoBool32, RendererDispatchIsPointWithinViewport, IsPointWithinViewport, const Vector3*, Vector3*)
LR2_BRIDGE2(LegoFloat, RendererDispatchScaleProjectionCoordinate, ScaleProjectionCoordinate, LegoFloat, LegoFloat)
LR2_BRIDGE2(LegoFloat, RendererDispatchTransformProjectedPoint, TransformProjectedPoint, const Vector3*, Vector3*)
LR2_BRIDGE2(LegoFloat, RendererDispatchTransformViewportPoint, TransformViewportPoint, const Vector3*, Vector3*)
LR2_BRIDGE0_VOID(RendererDispatchUpdateTransformCache, UpdateTransformCache)
LR2_BRIDGE1(LegoBool32, RendererDispatchShouldUseTextureResourceIndex, ShouldUseTextureResourceIndex, LegoU32)
LR2_BRIDGE1(LegoBool32, RendererDispatchIsTextureResourceKindAccepted, IsTextureResourceKindAccepted, LegoU32)
LR2_BRIDGE1(LegoBool32, RendererDispatchIsTextureResourceKindSpecial, IsTextureResourceKindSpecial, LegoU32)
LR2_BRIDGE1(LegoBool32, RendererDispatchAcceptUnusedRendererPolicy, AcceptUnusedRendererPolicy, LegoU32)
LR2_BRIDGE0(LegoBool32, RendererDispatchHasPreferredDepthFormat, HasPreferredDepthFormat)
LR2_BRIDGE1_VOID(RendererDispatchBuildBumpMapVectors, BuildBumpMapVectors, RendererStagePlanRequest*)
LR2_BRIDGE1_VOID(RendererDispatchSetStencilBufferEnabled, SetStencilBufferEnabled, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchResetStencilOperations, ResetStencilOperations, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSetStencilFunction, SetStencilFunction, LegoU32)
LR2_BRIDGE3_VOID(RendererDispatchSetStencilOperation, SetStencilOperation, LegoU32, LegoU32, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSetDepthState, SetDepthState, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSetAlphaTestReference, SetAlphaTestReference, LegoU32)
LR2_BRIDGE0_VOID(RendererDispatchSetDepthAlwaysWrite, SetDepthAlwaysWrite)
LR2_BRIDGE1_VOID(RendererDispatchAdjustSolidPassScope, AdjustSolidPassScope, LegoU32)
LR2_BRIDGE0(LegoBool32, RendererDispatchIsSolidPassOverrideActive, IsSolidPassOverrideActive)
LR2_BRIDGE3(LegoBool32, RendererDispatchRejectUnknownDrawRequest, RejectUnknownDrawRequest, LegoU32, LegoU32, LegoU32)
LR2_BRIDGE4(LegoS32, RendererDispatchAddViewport, AddViewport, LegoU32, LegoU32, LegoU32, LegoU32)
LR2_BRIDGE5_VOID(RendererDispatchUpdateViewport, UpdateViewport, LegoU32, LegoU32, LegoU32, LegoU32, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSelectViewport, SelectViewport, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchReleaseViewport, ReleaseViewport, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSetColorOverride, SetColorOverride, LegoU32)
LR2_BRIDGE0_VOID(RendererDispatchDisableColorOverride, DisableColorOverride)
LR2_BRIDGE1_VOID(RendererDispatchSetAlphaBlendMode, SetAlphaBlendMode, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSetBlendModeOverride, SetBlendModeOverride, LegoU32)
LR2_BRIDGE0_VOID(RendererDispatchClearBlendModeOverride, ClearBlendModeOverride)
LR2_BRIDGE7(
	LegoBool32,
	RendererDispatchCreateTextureRecord,
	CreateTextureRecord,
	LegoU32,
	LegoS32,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32*,
	LegoU32
)
LR2_BRIDGE3(
	LegoBool32,
	RendererDispatchBuildMipSidecar,
	BuildMipSidecar,
	const LegoChar*,
	const LegoChar*,
	BitmapImage*
)
LR2_BRIDGE4(
	LegoBool32,
	RendererDispatchLoadTextureImage,
	LoadTextureImage,
	LegoS32,
	BitmapImage*,
	LegoS32,
	const LegoChar*
)
LR2_BRIDGE1_VOID(RendererDispatchReleaseTextureRecord, ReleaseTextureRecord, LegoS32)
LR2_BRIDGE0(LegoBool32, RendererDispatchSupportsCubeTextures, SupportsCubeTextures)
LR2_BRIDGE4(
	LegoBool32,
	RendererDispatchLoadCubeTextureFace,
	LoadCubeTextureFace,
	LegoS32,
	BitmapImage*,
	const LegoChar*,
	LegoS32
)
LR2_BRIDGE1_VOID(RendererDispatchSetTextureRenderTarget, SetTextureRenderTarget, RendererTextureRecord*)
LR2_BRIDGE2_VOID(
	RendererDispatchDrawGuiRectangle,
	DrawGuiRectangle,
	const RendererGuiRectangle*,
	RendererDefaultRecord*
)
LR2_BRIDGE1_VOID(RendererDispatchDrawTerrain, DrawTerrain, Terrain*)
LR2_BRIDGE2(RendererIndexBuffer*, RendererDispatchCreateIndexBuffer, CreateIndexBuffer, LegoU32, LegoS32)
LR2_BRIDGE1_VOID(RendererDispatchSetFogEnabled, SetFogEnabled, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchSetFogColor, SetFogColor, LegoU32)
LR2_BRIDGE2_VOID(RendererDispatchSetFogRange, SetFogRange, LegoFloat, LegoFloat)
LR2_BRIDGE1(LegoU32, RendererDispatchGetDotProductStageFlag, GetDotProductStageFlag, LegoU32)
LR2_BRIDGE1(RendererTextureRecord*, RendererDispatchGetTextureRecord, GetTextureRecord, LegoU32)
LR2_BRIDGE2(
	RendererAnimationResourceRecord*,
	RendererDispatchFindOrAllocateAnimationResource,
	FindOrAllocateAnimationResource,
	const LegoChar*,
	LegoBool32
)
LR2_BRIDGE2_VOID(RendererDispatchSetTextureIndexMap, SetTextureIndexMap, LegoU32, LegoU32)
LR2_BRIDGE2(LegoU32, RendererDispatchUpdateTextureStageFlags, UpdateTextureStageFlags, RendererDefaultSlot*, LegoU32)
LR2_BRIDGE1_VOID(RendererDispatchApplyTextureOperation, ApplyTextureOperation, LegoU32)
LR2_BRIDGE0(LegoFloat, RendererDispatchGetStageBlend, GetStageBlend)
LR2_BRIDGE1_VOID(RendererDispatchSetStageBlend, SetStageBlend, LegoFloat)
LR2_BRIDGE1_VOID(RendererDispatchGetTimingMetrics, GetTimingMetrics, LegoFloat*)
LR2_BRIDGE0(LegoU32, RendererDispatchGetMaximumTextureDimension, GetMaximumTextureDimension)
LR2_BRIDGE2_VOID(RendererDispatchDrawUnlitLines, DrawUnlitLines, const Vector3*, LegoS32)
LR2_BRIDGE2_VOID(RendererDispatchDrawUnlitTriangles, DrawUnlitTriangles, const void*, LegoS32)
LR2_BRIDGE1_VOID(RendererDispatchDrawQuadPacket, DrawQuadPacket, const void*)
LR2_BRIDGE4_VOID(RendererDispatchBeginSpriteBatch, BeginSpriteBatch, LegoS32, LegoU32, LegoFloat, LegoFloat)
LR2_BRIDGE1_VOID(RendererDispatchDrawSpriteGeometry, DrawSpriteGeometry, RendererSpriteRecord*)
LR2_BRIDGE1_VOID(RendererDispatchDrawSprite, DrawSprite, RendererSpriteRecord*)
LR2_BRIDGE1_VOID(RendererDispatchDrawStencilVolumes, DrawStencilVolumes, LegoS32)
LR2_BRIDGE0_VOID(RendererDispatchDrawStencilOverlay, DrawStencilOverlay)
LR2_BRIDGE0(RendererVertexBuffer*, RendererDispatchGetQuadVertexBuffer, GetQuadVertexBuffer)
LR2_BRIDGE6(
	LegoBool32,
	RendererDispatchSupportsUnknownDrawFeature,
	SupportsUnknownDrawFeature,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
)
LR2_BRIDGE3_VOID(RendererDispatchSetTextureTransform, SetTextureTransform, LegoS32, const Matrix3*, const Vector3*)
LR2_BRIDGE0(LegoU32, RendererDispatchGetSelectedAdapterModeCount, GetSelectedAdapterModeCount)
LR2_BRIDGE5_VOID(
	RendererDispatchGetSelectedAdapterMode,
	GetSelectedAdapterMode,
	LegoU32,
	LegoU32*,
	LegoU32*,
	LegoU32*,
	LegoU32*
)
LR2_BRIDGE4(
	LegoBool32,
	RendererDispatchReconfigureDisplayMode,
	ReconfigureDisplayMode,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoS32
)
LR2_BRIDGE0(LegoBool32, RendererDispatchShouldUseHighResolutionTextures, ShouldUseHighResolutionTextures)
LR2_BRIDGE1_VOID(RendererDispatchSetClockwiseCulling, SetClockwiseCulling, LegoBool32)
LR2_BRIDGE1_VOID(RendererDispatchSetUnknownDrawPolicy, SetUnknownDrawPolicy, LegoU32)
LR2_BRIDGE0(LegoS32, RendererDispatchGetTextureQuality, GetTextureQuality)
LR2_BRIDGE1_VOID(RendererDispatchSetTextureQuality, SetTextureQuality, LegoU32)
LR2_BRIDGE0(LegoBool32, RendererDispatchAdvanceDeviceFallback, AdvanceDeviceFallback)
LR2_BRIDGE0(LegoBool32, RendererDispatchSupportsExtendedTextureStages, SupportsExtendedTextureStages)
LR2_BRIDGE0(LegoS32, RendererDispatchGetDeviceMemoryMegabytes, GetDeviceMemoryMegabytes)
LR2_BRIDGE0(LegoBool32, RendererDispatchUsesHardwareVertexProcessing, UsesHardwareVertexProcessing)

namespace
{
typedef void(__cdecl* Purecall)(void);

#define LR2_PURE(TypeName) reinterpret_cast<RendererMainDispatch::TypeName>(_purecall)

} // namespace

const RendererBaseDispatch g_rendererBaseDispatch = {
	LR2_PURE(Predicate),
	LR2_PURE(Action),
	LR2_PURE(PredicateU32),
	LR2_PURE(Predicate),
	LR2_PURE(Action),
	reinterpret_cast<RendererMainDispatch::Action>(ApplicationNoOp),
	reinterpret_cast<RendererMainDispatch::Action>(ApplicationNoOp),
	LR2_PURE(SetU32),
	LR2_PURE(PredicateU32),
	LR2_PURE(Predicate),
	LR2_PURE(SetProjection),
	LR2_PURE(SetQuaternionTranslation),
	LR2_PURE(SetRotationTranslation),
	LR2_PURE(Action),
	LR2_PURE(SetRotationTranslation),
	LR2_PURE(SelectLights),
	LR2_PURE(FloatPointer),
	LR2_PURE(CreateVertexBuffer),
	LR2_PURE(SetStageRequest),
	LR2_PURE(SetDrawPacket),
	LR2_PURE(SetDrawPacket),
	LR2_PURE(PredicatePointer2),
	LR2_PURE(ScaleCoordinate),
	LR2_PURE(TransformWorldView),
	LR2_PURE(TransformPoint),
	LR2_PURE(TransformPoint),
	LR2_PURE(Action),
	LR2_PURE(PredicateU32),
	LR2_PURE(PredicateU32),
	LR2_PURE(PredicateU32),
	LR2_PURE(PredicateU32),
	reinterpret_cast<RendererMainDispatch::Predicate>(IsApplicationDevelopmentMode),
	LR2_PURE(SetStageRequest),
	LR2_PURE(SetStageRequest),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(Set3U32),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(Action),
	LR2_PURE(SetU32),
	LR2_PURE(Predicate),
	reinterpret_cast<RendererMainDispatch::Predicate3U32>(RendererDispatchRejectUnknownDrawRequest),
	LR2_PURE(AddViewport),
	LR2_PURE(UpdateViewport),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(Action),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(Action),
	LR2_PURE(CreateTextureRecord),
	LR2_PURE(BuildMipSidecar),
	LR2_PURE(GenerateNormalMap),
	LR2_PURE(LoadTextureImage),
	LR2_PURE(SetS32),
	LR2_PURE(Predicate),
	LR2_PURE(LoadCubeFace),
	LR2_PURE(SetTextureRecord),
	LR2_PURE(SetGuiRectangle),
	LR2_PURE(DrawTerrain),
	LR2_PURE(TerrainGeometry),
	LR2_PURE(CreateIndexBuffer),
	LR2_PURE(SetU32),
	LR2_PURE(SetU32),
	LR2_PURE(SetFloat2),
	LR2_PURE(ValueU32),
	LR2_PURE(TextureRecordIndex),
	LR2_PURE(AnimationResource),
	LR2_PURE(SetTextureIndexMap),
	LR2_PURE(UpdateTextureStageFlags),
	LR2_PURE(SetU32),
	LR2_PURE(FloatValue),
	LR2_PURE(SetFloat),
	LR2_PURE(GetMetrics),
	LR2_PURE(Value),
	LR2_PURE(DrawLines),
	LR2_PURE(DrawTriangles),
	LR2_PURE(SetPointer),
	LR2_PURE(SpriteBatch),
	LR2_PURE(DrawSprite),
	LR2_PURE(DrawSprite),
	LR2_PURE(SetS32),
	LR2_PURE(Action),
	LR2_PURE(VertexBufferPointer),
	LR2_PURE(Predicate6U32),
	LR2_PURE(Diagnostic),
	LR2_PURE(Predicate3U32),
	LR2_PURE(SetTextureTransform),
	LR2_PURE(GetModeCount),
	LR2_PURE(GetMode),
	LR2_PURE(DisplayMode),
	reinterpret_cast<RendererMainDispatch::Predicate>(IsApplicationDevelopmentMode),
	reinterpret_cast<RendererMainDispatch::SetBool>(OnApplicationDiagnostic),
	reinterpret_cast<RendererMainDispatch::SetU32>(OnApplicationDiagnostic),
	reinterpret_cast<RendererMainDispatch::SignedValue>(ApplicationTruePolicy),
	reinterpret_cast<RendererMainDispatch::SetU32>(OnApplicationDiagnostic),
	reinterpret_cast<RendererMainDispatch::Predicate>(IsApplicationDevelopmentMode),
	reinterpret_cast<RendererMainDispatch::Predicate>(IsApplicationDevelopmentMode),
	reinterpret_cast<RendererMainDispatch::SignedValue>(IsApplicationDevelopmentMode),
	reinterpret_cast<RendererMainDispatch::Predicate>(IsApplicationDevelopmentMode)
};

const RendererMainDispatch g_rendererDerivedDispatch = {
	RendererDispatchInitialize,
	RendererDispatchShutdown,
	RendererDispatchSetDeviceResourceMode,
	RendererDispatchBeginScene,
	RendererDispatchEndScene,
	RendererDispatchNoOp,
	RendererDispatchNoOp,
	RendererDispatchSetMaterialColor,
	RendererDispatchClearBuffers,
	RendererDispatchPresent,
	RendererDispatchSetProjection,
	RendererDispatchSetWorldQuaternion,
	RendererDispatchSetWorldMatrix,
	ApplyDefaultWorldTransform,
	RendererDispatchSetViewMatrix,
	RendererDispatchSelectLights,
	RendererDispatchGetSelectedLightDirection,
	RendererDispatchCreateVertexBuffer,
	RendererDispatchPrepareDrawPlan,
	RendererDispatchDrawRendererPacket,
	RendererDispatchDrawIndexedPacket,
	RendererDispatchIsPointWithinViewport,
	RendererDispatchScaleProjectionCoordinate,
	TransformStaticWorldViewPoint,
	RendererDispatchTransformProjectedPoint,
	RendererDispatchTransformViewportPoint,
	RendererDispatchUpdateTransformCache,
	RendererDispatchShouldUseTextureResourceIndex,
	RendererDispatchIsTextureResourceKindAccepted,
	RendererDispatchIsTextureResourceKindSpecial,
	RendererDispatchAcceptUnusedRendererPolicy,
	RendererDispatchHasPreferredDepthFormat,
	ResolveRendererTextureStagePlan,
	RendererDispatchBuildBumpMapVectors,
	RendererDispatchSetStencilBufferEnabled,
	RendererDispatchResetStencilOperations,
	RendererDispatchSetStencilFunction,
	RendererDispatchSetStencilOperation,
	RendererDispatchSetDepthState,
	RendererDispatchSetAlphaTestReference,
	RendererDispatchSetDepthAlwaysWrite,
	RendererDispatchAdjustSolidPassScope,
	RendererDispatchIsSolidPassOverrideActive,
	RendererDispatchRejectUnknownDrawRequest,
	RendererDispatchAddViewport,
	RendererDispatchUpdateViewport,
	RendererDispatchSelectViewport,
	RendererDispatchReleaseViewport,
	RendererDispatchSetColorOverride,
	RendererDispatchDisableColorOverride,
	RendererDispatchSetAlphaBlendMode,
	RendererDispatchSetBlendModeOverride,
	RendererDispatchClearBlendModeOverride,
	RendererDispatchCreateTextureRecord,
	RendererDispatchBuildMipSidecar,
	GenerateNormalMap,
	RendererDispatchLoadTextureImage,
	RendererDispatchReleaseTextureRecord,
	RendererDispatchSupportsCubeTextures,
	RendererDispatchLoadCubeTextureFace,
	RendererDispatchSetTextureRenderTarget,
	RendererDispatchDrawGuiRectangle,
	RendererDispatchDrawTerrain,
	BuildTerrainGeometry,
	RendererDispatchCreateIndexBuffer,
	RendererDispatchSetFogEnabled,
	RendererDispatchSetFogColor,
	RendererDispatchSetFogRange,
	RendererDispatchGetDotProductStageFlag,
	RendererDispatchGetTextureRecord,
	RendererDispatchFindOrAllocateAnimationResource,
	RendererDispatchSetTextureIndexMap,
	RendererDispatchUpdateTextureStageFlags,
	RendererDispatchApplyTextureOperation,
	RendererDispatchGetStageBlend,
	RendererDispatchSetStageBlend,
	RendererDispatchGetTimingMetrics,
	RendererDispatchGetMaximumTextureDimension,
	RendererDispatchDrawUnlitLines,
	RendererDispatchDrawUnlitTriangles,
	RendererDispatchDrawQuadPacket,
	RendererDispatchBeginSpriteBatch,
	RendererDispatchDrawSpriteGeometry,
	RendererDispatchDrawSprite,
	RendererDispatchDrawStencilVolumes,
	RendererDispatchDrawStencilOverlay,
	RendererDispatchGetQuadVertexBuffer,
	RendererDispatchSupportsUnknownDrawFeature,
	OnApplicationDiagnostic,
	RendererDispatchRejectUnknownDrawRequest,
	RendererDispatchSetTextureTransform,
	RendererDispatchGetSelectedAdapterModeCount,
	RendererDispatchGetSelectedAdapterMode,
	RendererDispatchReconfigureDisplayMode,
	RendererDispatchShouldUseHighResolutionTextures,
	RendererDispatchSetClockwiseCulling,
	RendererDispatchSetUnknownDrawPolicy,
	RendererDispatchGetTextureQuality,
	RendererDispatchSetTextureQuality,
	RendererDispatchAdvanceDeviceFallback,
	RendererDispatchSupportsExtendedTextureStages,
	RendererDispatchGetDeviceMemoryMegabytes,
	RendererDispatchUsesHardwareVertexProcessing,
};

#undef LR2_PURE
#undef LR2_BRIDGE0
#undef LR2_BRIDGE1
#undef LR2_BRIDGE2
#undef LR2_BRIDGE3
#undef LR2_BRIDGE4
#undef LR2_BRIDGE5
#undef LR2_BRIDGE6
#undef LR2_BRIDGE7
