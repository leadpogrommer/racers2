#ifndef LR2_RENDERER_H
#define LR2_RENDERER_H

#include "decomp.h"
#include "directx8.h"
#include "pixelformat.h"
#include "rendereranimationresource.h"
#include "rendererbuffer.h"
#include "rendererbufferobjects.h"
#include "rendererdispatch.h"
#include "rendererrecords.h"
#include "renderertexturemanager.h"
#include "renderertimingstatistics.h"
#include "types.h"

#include <stddef.h>

struct RendererAdapterRecord {
	LegoU32 m_adapterIndex;
	LegoChar m_driverName[512];
	LegoChar m_description[512];
	D3DCAPS8 m_caps;
};

DECOMP_SIZE_ASSERT(RendererAdapterRecord, 0x4d8);

struct RendererViewportRecord {
	LegoU32 m_inUse;
	D3DVIEWPORT8 m_viewport;
};
DECOMP_SIZE_ASSERT(RendererViewportRecord, 0x1c);

struct RendererLightRecord {
	LegoU16 m_flags;
	LegoU16 m_deviceIndex;
	LegoFloat m_radius;
	D3DLIGHT8 m_light;
};
DECOMP_SIZE_ASSERT(D3DLIGHT8, 0x68);
DECOMP_SIZE_ASSERT(RendererLightRecord, 0x70);

struct RendererTextureFormat {
	D3DFORMAT m_format;
	LegoU32 m_selectedAsCompressed; // 0x04, set when a DXT preference is selected
	PixelFormatDescription m_description;
};

DECOMP_SIZE_ASSERT(RendererTextureFormat, 0x28);

struct RendererTextureTransform {
	LegoU32 m_used;
	D3DMATRIX m_matrix;
};
DECOMP_SIZE_ASSERT(RendererTextureTransform, 0x44);

struct RendererResourceWorkList {
	void* m_elements;
	LegoU32 m_stride;
	LegoU32 m_count;
	LegoU32 m_capacity;
};
DECOMP_SIZE_ASSERT(RendererResourceWorkList, 0x10);

// This is an evidenced prefix of the static renderer object at 0x0058b140.
// The full object extends beyond this prefix; its complete size is not known.
class Matrix3;
class Quaternion;
class Vector3;
class BitmapImage;
class Terrain;
class TerrainVertexGrid;

class DisplayRenderer {
public:
	DisplayRenderer();
	void PrepareDrawPacketState(struct RendererDrawPacketView* p_packet);
	void DrawTerrain(Terrain* p_terrain);
	void BuildTerrainGridStream(
		LegoU8** p_vertexCursor,
		TerrainVertexGrid* p_grid,
		LegoS32 p_level,
		LegoU32* p_indexOffset,
		LegoU32* p_vertexBase,
		LegoS32 p_topology
	);
	void ReapplyLights();
	void GetLightData(LegoS32 p_index, D3DLIGHT8** p_light);
	void UpdateLightPose(LegoS32 p_index, const Vector3* p_position, const Vector3* p_direction, LegoFloat p_radius);
	RendererLightRecord* GetLightRecord(LegoS32 p_index);
	LegoFloat GetMaximumDirectionalLightIntensity();
	RendererAuxiliaryResourceRecord* GetAuxiliaryResource(LegoS32 p_index);
	void ApplyAuxiliaryMaterial(LegoS32 p_index);
	void SetAmbientColorFloats(const LegoFloat* p_color);
	LegoBool32 SetDeviceResourceMode(LegoU32 p_mode);
	LegoBool32 AdvanceDeviceFallback();
	LegoBool32 ReconfigureDisplayMode(LegoU32 p_width, LegoU32 p_height, LegoU32 p_bitDepth, LegoS32 p_refreshRate);
	LegoBool32 AcceptUnusedRendererPolicy(LegoU32 p_unused);
	void BuildBumpMapVectors(struct RendererStagePlanRequest* p_request);
	void DrawGuiRectangle(const struct RendererGuiRectangle* p_rectangle, RendererDefaultRecord* p_defaults);
	void DrawQuadPacket(const void* p_packet);
	void DrawRendererPacket(struct RendererDrawPacketView* p_packet);
	void DrawIndexedPacket(struct RendererDrawPacketView* p_packet);
	void RefreshTextureFormats();
	LegoBool32 IsTextureFormatSupported(D3DFORMAT p_format);
	RendererAnimationResourceRecord* FindOrAllocateAnimationResource(const LegoChar* p_path, LegoBool32 p_loadNow);
	void DrawStencilOverlay();
	void DrawUnlitLines(const Vector3* p_vertices, LegoS32 p_count);
	void DrawUnlitTriangles(const void* p_vertices, LegoS32 p_count);
	void ReleaseDeviceResources();
	LegoBool32 RestoreDeviceResources();
	LegoBool32 BeginScene();
	void DrawLookupEntry(RendererLookupTableState::Entry* p_entry);
	void BeginSpriteBatch(LegoS32 p_textureId, LegoU32 p_blendMode, LegoFloat p_unused0, LegoFloat p_unused1);
	void DrawSprite(struct RendererSpriteRecord* p_sprite);
	void DrawSpriteGeometry(struct RendererSpriteRecord* p_sprite);
	struct RendererSpriteVertex* AllocateSpriteVertices();
	void SetTextureTransform(LegoS32 p_index, const Matrix3* p_rotation, const Vector3* p_translation);
	void SetTextureRenderTarget(RendererTextureRecord* p_record);
	void DrawStencilVolumes(LegoS32 p_vertexCount);
	void PrepareDrawPlan(struct RendererStagePlanRequest* p_request);
	LegoBool32 SupportsUnknownDrawFeature(LegoU32, LegoU32, LegoU32, LegoU32, LegoU32, LegoU32);
	LegoBool32 RejectUnknownDrawRequest(LegoU32 p_arg0, LegoU32 p_arg1, LegoU32 p_arg2);
	LegoBool32 IsPointWithinViewport(const Vector3* p_source, Vector3* p_result);
	void GetTimingMetrics(LegoFloat p_metrics[6]);
	void SetTextureQuality(LegoU32 p_quality);
	void SetFogEnabled(LegoU32 p_enabled);
	void SetFogColor(LegoU32 p_color);
	void SetFogRange(LegoFloat p_start, LegoFloat p_end);
	void SetAlphaTestReference(LegoU32 p_reference);
	void SetDepthAlwaysWrite();
	void AdjustSolidPassScope(LegoU32 p_enter);
	LegoBool32 IsSolidPassOverrideActive();
	LegoBool32 BuildMipSidecar(
		const LegoChar* p_sourcePath,
		const LegoChar* p_outputSuffix,
		BitmapImage* p_sourceImage
	);
	void PrepareBitmapForTexture(BitmapImage* p_image);
	void UpdateTransformCache();
	void EndScene();
	LegoFloat ScaleProjectionCoordinate(LegoFloat p_scale, LegoFloat p_depth);
	LegoFloat TransformProjectedPoint(const Vector3* p_source, Vector3* p_result);
	LegoFloat TransformViewportPoint(const Vector3* p_source, Vector3* p_result);
	LegoBool32 LoadTextureImage(
		LegoS32 p_id,
		BitmapImage* p_image,
		LegoS32 p_resourceType,
		const LegoChar* p_sourcePath
	);
	LegoBool32 LoadCubeTextureFace(
		LegoS32 p_id,
		BitmapImage* p_image,
		const LegoChar* p_sourcePath,
		LegoS32 p_faceIndex
	);
	RendererResourceRef* FindTextureReferenceById(LegoS32 p_id, LegoU32 p_type);
	void ReleaseTextureResourceReference(LegoS32 p_id, LegoU32 p_type);
	LegoBool32 InitializeResources();
	void ShutdownResources();
	void ReleaseAuxiliaryResource(LegoS32 p_index);
	void UpdateAnimatedResources();
	void ReleaseTextureRecord(LegoS32 p_recordIndex);
	void Shutdown();
	void ReleaseViewport(LegoU32 p_index);
	LegoFloat* GetSelectedLightDirection();
	LegoBool32 HasPreferredDepthFormat();
	LegoFloat GetStageBlend();
	void SetStageBlend(LegoFloat p_blend);
	LegoU32 GetMaximumTextureDimension();
	RendererVertexBuffer* GetQuadVertexBuffer();
	void SetClockwiseCulling(LegoBool32 p_enabled);
	void SetUnknownDrawPolicy(LegoU32 p_value);
	LegoU32 GetDotProductStageFlag(LegoU32 p_flags);
	LegoBool32 SupportsExtendedTextureStages();
	LegoS32 GetDeviceMemoryMegabytes();
	LegoBool32 CreateTextureRecord(
		LegoU32 p_createMode,
		LegoS32 p_recordIndex,
		LegoU32 p_width,
		LegoU32 p_height,
		LegoU32 p_mipLevels,
		LegoU32* p_compressedOutput,
		LegoU32 p_flagA
	);
	void ApplyTextureTransform(const RendererDefaultSlot* p_slot, LegoU32 p_stage);
	LegoU32 GetStageFactorColor();
	LegoU32 GetInverseStageFactorColor();
	LegoBool32 SupportsCubeTextures();
	LegoBool32 SupportsSharedMemoryMultitexturing();
	LegoU32 UpdateTextureStageFlags(RendererDefaultSlot* p_slot, LegoU32 p_mode);
	void SetTextureIndexMap(LegoU32 p_index, LegoU32 p_value);
	LegoBool32 ShouldUseHighResolutionTextures();
	LegoBool32 ShouldUseTextureResourceIndex(LegoU32 p_kind);
	LegoBool32 IsTextureResourceKindAccepted(LegoU32 p_kind);
	LegoBool32 IsTextureResourceKindSpecial(LegoU32 p_kind);
	LegoBool32 SupportsDotProduct3();
	void CopyBitmapMipToLockedSurface(
		LegoU32 p_mipLevel,
		const BitmapImage* p_image,
		const RendererTextureFormat* p_format,
		const D3DLOCKED_RECT* p_locked
	);
	LegoBool32 CreatePrimaryTexture(RendererTextureRecord* p_record, LegoBool32 p_compressed);
	LegoBool32 CreateSecondaryTexture(RendererTextureRecord* p_record);
	LegoS32 GetTextureMipLevelCount(LegoS32 p_width, LegoS32 p_height) const;
	void AdjustTextureDimensions(LegoU32* p_width, LegoU32* p_height, LegoU32 p_ignoredFormat);
	LegoBool32 SetTextureRecordDimensions(
		RendererTextureRecord* p_record,
		LegoU32 p_width,
		LegoU32 p_height,
		LegoU32 p_mipLevels,
		LegoU32 p_flagA,
		LegoU8 p_sevenBitField,
		LegoU32 p_formatMode
	);
	LegoBool32 Initialize();
	void ConfigureStreamResources(LegoU32 p_enabled);
	RendererVertexBuffer* CreateVertexBuffer(
		LegoU32 p_attributes,
		LegoS32 p_count,
		LegoS32 p_textureCount,
		LegoU32 p_mode
	);
	LegoBool32 TryReuseVertexStorage(
		LegoU32 p_attributes,
		LegoS32 p_count,
		LegoS32 p_textureCount,
		LegoU32 p_mode,
		RendererVertexBuffer* p_buffer
	);
	RendererVertexStorage* AllocateVertexStorage(
		LegoU32 p_attributes,
		LegoS32 p_count,
		LegoS32 p_textureCount,
		LegoU32 p_mode
	);
	RendererIndexBuffer* CreateIndexBuffer(LegoU32 p_kind, LegoS32 p_count);
	void SelectLights(const Vector3* p_center, LegoFloat p_radius, LegoU32 p_mode, LegoU32* p_maskAndDirection);
	void SetProjection(
		LegoFloat p_orthographicScale,
		LegoFloat p_horizontalFov,
		LegoFloat p_verticalFov,
		LegoFloat p_nearPlane,
		LegoFloat p_farPlane,
		LegoU32 p_mode
	);
	void SetViewTransform(const Matrix3* p_rotation, const Vector3* p_position);
	void SetWorldTransform(const Matrix3* p_rotation, const Vector3* p_position);
	void SetWorldTransform(const Quaternion* p_rotation, const Vector3* p_position);
	LegoS32 AcquireAuxiliaryResource(RendererAuxiliaryResourceRecord* p_record, LegoU32 p_resourceType);
	RendererTextureRecord* FindOrCreateTextureRecord(
		const LegoChar* p_path,
		LegoU32 p_kind,
		LegoS32 p_index,
		LegoU32 p_resourceMode,
		LegoU32 p_auxiliaryMode,
		LegoBool32 p_load
	);
	LegoS32 CreateTextureResource(
		const LegoChar* p_path,
		LegoU32 p_resourceKind,
		LegoS32 p_resourceIndex,
		LegoU32 p_createMode,
		LegoU16 p_referenceType
	);
	RendererResourceCacheEntry* FindOrAllocateTextureResource(const LegoChar* p_path, LegoBool32 p_loadNow);
	LegoS32 AddTextureReference(void* p_resource, LegoU16 p_resourceType);
	RendererResourceRef* FindTextureReference(void* p_resource);
	void SetStencilBufferEnabled(LegoU32 p_enabled);
	void ResetStencilOperations(LegoU32 p_ignored);
	void SetStencilFunction(LegoU32 p_function);
	void SetStencilOperation(LegoU32 p_target, LegoU32 p_operation, LegoU32 p_reference);
	void SetColorOverride(LegoU32 p_color);
	void DisableColorOverride();
	void SetBlendModeOverride(LegoU32 p_mode);
	void ClearBlendModeOverride();
	void SetVertexStream(IDirect3DVertexBuffer8* p_buffer, LegoU32 p_stride, LegoU32 p_shader);
	void ApplyTextureOperation(LegoU32 p_operation);
	void BindMappedTexture(LegoS32 p_slot);
	void RestoreTextureStageDefaults();
	RendererTextureRecord* GetTextureRecord(LegoU32 p_index);
	LegoS32 AddViewport(LegoU32 p_x, LegoU32 p_y, LegoU32 p_width, LegoU32 p_height);
	void UpdateViewport(LegoU32 p_index, LegoU32 p_x, LegoU32 p_y, LegoU32 p_width, LegoU32 p_height);
	void SelectViewport(LegoU32 p_index);
	void SetProjectionFromViewport(const D3DVIEWPORT8* p_viewport);
	void SetDepthState(LegoU32 p_enabled);
	void SetAlphaBlendMode(LegoU32 p_mode);
	void SetMaterialColor(LegoU32 p_color);
	LegoBool32 ClearBuffers(LegoU32 p_flags);
	LegoBool32 Present();
	LegoS32 GetTextureQuality();
	void AddSupportedTextureFormat(D3DFORMAT p_format);
	LegoBool32 InitializeTextureFormats();
	LegoS32 SelectTextureFormat(LegoU32 p_flags, LegoU32 p_compressed, LegoU32 p_force32Bit);
	LegoBool32 EnumerateSupportedAdapters();
	void ConfigureNvidiaWorkaround();
	LegoBool32 SelectDepthStencilFormat();
	LegoBool32 UsesHardwareVertexProcessing();
	LegoBool32 CreateVertexBuffers();
	void InitializeDefaultRenderStates();
	void SetCachedRenderState(D3DRENDERSTATETYPE p_state, LegoU32 p_value);
	void ResetTextureStages();
	void SetCachedTextureStageState(LegoU32 p_stage, D3DTEXTURESTAGESTATETYPE p_state, LegoU32 p_value);
	void SetTextureFiltering(LegoU32 p_linear);
	LegoBool32 FindBestDisplayMode(LegoU32* p_width, LegoU32* p_height, LegoU32* p_bitDepth, LegoU32* p_refreshRate);
	LegoU32 GetSelectedAdapterModeCount();
	void GetSelectedAdapterMode(
		LegoU32 p_modeIndex,
		LegoU32* p_width,
		LegoU32* p_height,
		LegoU32* p_bitDepth,
		LegoU32* p_refreshRate
	);

	const RendererDispatchView* m_vtable;                  // 0x000
	RendererLookupTableState m_bucketedDraws;              // 0x04
	RendererLookupTableState m_orderedDraws;               // 0x2c
	RendererLookupTableState m_deferredDraws;              // 0x54
	RendererLookupTableState* m_drawLookupTables[3];       // 0x7c
	RendererAuxiliaryResourceRecord* m_auxiliaryResources; // 0x88
	RendererResourceRef* m_resourceReferences;             // 0x8c
	LegoS32 m_resourceReferenceCount;                      // 0x90
	RendererResourceCacheEntry* m_resourceCache;           // 0x94
	RendererResourceRef** m_resources0x98;                 // 0x98
	LegoS32 m_resourceCount0x9c;                           // 0x9c
	void** m_resources0xa0;                                // 0xa0
	LegoS32 m_resourceCount0xa4;                           // 0xa4
	RendererResourceLookupEntry* m_resourceLookup;         // 0xa8
	LegoS32 m_resourceLookupCount;                         // 0xac
	LegoU32 m_resourceLoadMode;                            // 0xb0
	RendererTextureRecord* m_textureHash[256];             // 0xb4
	void* m_unk0x4b4;
	undefined m_unk0x4b8[0x4c0 - 0x4b8];
	LegoU32 m_unk0x4c0;
	RendererResourceWorkList* m_resourceWorkList; // 0x4c4
	LegoU32 m_unk0x4c8;
	void* m_unk0x4cc;
	LegoS32 m_unk0x4d0;
	LegoS32 m_unk0x4d4;
	LegoS32 m_unk0x4d8;
	LegoU32 m_unk0x4dc;
	LegoS32 m_cachedTextureRecordIds[6]; // 0x4e0
	LegoU32 m_unk0x4f8;
	LegoU32 m_textureAddressModes[16]; // 0x4fc
	D3DMATRIX m_transform0x53c;        // 0x53c
	D3DMATRIX m_worldTransform;        // 0x57c
	D3DMATRIX m_viewTransform;         // 0x5bc
	D3DMATRIX m_projectionTransform;   // 0x5fc
	D3DMATRIX m_viewportTransform;     // 0x63c
	D3DMATRIX m_transform0x67c;        // 0x67c
	D3DMATRIX m_transform0x6bc;        // 0x6bc
	D3DMATRIX m_transform0x6fc;        // 0x6fc
	D3DMATRIX m_defaultWorldTransform; // 0x73c
	LegoFloat m_nearClip;              // 0x77c
	LegoFloat m_clearQuadDepth;        // 0x780
	LegoFloat m_projectionPixelScale;  // 0x784
	LegoU32 m_unk0x788;
	LegoFloat m_viewportX;                          // 0x78c
	LegoFloat m_viewportY;                          // 0x790
	LegoFloat m_viewportRight;                      // 0x794
	LegoFloat m_viewportBottom;                     // 0x798
	LegoFloat m_viewportWidth;                      // 0x79c
	LegoFloat m_viewportHeight;                     // 0x7a0
	LegoFloat m_viewportHalfWidth;                  // 0x7a4
	LegoFloat m_viewportHalfHeight;                 // 0x7a8
	IDirect3D8* m_direct3D;                         // 0x7ac
	IDirect3DDevice8* m_device;                     // 0x7b0
	LegoU32 m_requestedAdapterIndex;                // 0x7b4
	D3DPRESENT_PARAMETERS m_presentationParameters; // 0x7b8..0x7eb
	LegoS32 m_textureQuality;                       // 0x7ec
	LegoU32 m_deviceBehaviorFlags;                  // 0x7f0, passed to IDirect3D8::CreateDevice
	LegoU32 m_restoreCursorOnShutdown;              // 0x7f4
	LegoU32 m_modeWidth;                            // 0x7f8
	LegoU32 m_modeHeight;                           // 0x7fc
	LegoU32 m_modeRefreshRate;                      // 0x800
	D3DFORMAT m_modeFormat;                         // 0x804, raw format rather than derived bit depth
	LegoU32 m_requestedBitDepth;                    // 0x808
	LegoBool32 m_clockwiseCulling;                  // 0x80c, chooses D3DCULL_CW rather than CCW
	LegoU32 m_unk0x810;
	LegoU32 m_deviceMemory; // 0x814, GetAvailableTextureMem
	LegoU32 m_unk0x818;
	LegoU32 m_totalPhysicalMemory;                                // 0x81c
	RendererAdapterRecord* m_selectedAdapter;                     // 0x820
	RendererTextureFormat* m_selectedTextureFormats[8];           // 0x824
	RendererTextureFormat* m_selectedCompressedTextureFormats[8]; // 0x844
	RendererAdapterRecord m_adapters[10];                         // 0x864
	LegoU32 m_adapterCount;                                       // 0x38d4
	RendererTextureRecord m_textureRecords[800];                  // 0x38d8
	LegoU32 m_defaultTextureIndex;                                // 0x22cd8
	LegoU32 m_textureIndexMap[800];                               // 0x22cdc
	LegoS32 m_textureFormatCount;                                 // 0x2395c
	RendererTextureFormat m_textureFormats[64];                   // 0x23960
	RendererViewportRecord m_viewports[16];                       // 0x24360
	LegoU32 m_clearRenderTarget;                                  // 0x24520
	LegoU32 m_currentViewportIndex;                               // 0x24524
	undefined m_unk0x24528[0x24544 - 0x24528];
	LegoFloat m_stageAlpha; // 0x24544
	undefined m_unk0x24548[0x2456c - 0x24548];
	LegoU32 m_vertexBufferUsage;            // 0x2456c, D3DUSAGE_WRITEONLY or zero
	LegoBool32 m_foundPreferredDepthFormat; // 0x24570, selected D24S8
	LegoU32 m_unk0x24574;                   // 0x24574
	LegoFloat m_ambientColorFloats[4];      // 0x24578
	undefined m_unk0x24588[0x24598 - 0x24588];
	LegoU32 m_ambientColor;     // 0x24598, D3DRS_AMBIENT default
	LegoU32 m_stageFactorColor; // 0x2459c
	D3DMATERIAL8 m_material;    // 0x245a0
	undefined m_unk0x245e4[0x245e8 - 0x245e4];
	RendererTimingStatisticsState m_timingStatistics;  // 0x245e8
	RendererTextureStageManager m_textureStageManager; // 0x25148
	RendererDefaultRecord m_textureDefaults;           // 0x28740
	RendererVertexBuffer m_vertexRecords[5000];        // 0x28768
	RendererVertexStorage m_vertexStorage[128];        // 0x8a1e8
	RendererIndexBuffer m_indexRecords[5000];          // 0x8b3e8
	RendererIndexStorage m_indexStorage[64];           // 0xc5d68
	LegoU32 m_unk0xc6268;
	LegoU32 m_fillMode; // 0xc626c
	LegoU32 m_unk0xc6270;
	LegoU32 m_colorOverrideEnabled;                   // 0xc6274
	LegoFloat m_colorOverride[4];                     // 0xc6278
	LegoU32 m_useBlendModeOverride;                   // 0xc6288
	LegoU32 m_blendModeOverride;                      // 0xc628c
	RendererLightRecord m_lights[32];                 // 0xc6290
	LegoS32 m_lightCount;                             // 0xc7090
	LegoFloat m_lightDirection[3];                    // 0xc7094
	LegoU32 m_textureTransformsEnabled;               // 0xc70a0
	RendererTextureTransform m_textureTransforms[10]; // 0xc70a4
	LegoU32 m_cachedRenderStates[0xac];               // 0xc734c
	LegoU32 m_cachedTextureHandles[8];                // 0xc75fc
	IDirect3DVertexBuffer8* m_cachedStreamBuffer;     // 0xc761c
	LegoU32 m_cachedVertexShader;                     // 0xc7620
	LegoU32 m_cachedTextureStageStates[256];          // 0xc7624, (stage << 5) | state
	LegoS32 m_unk0xc7a24;
	LegoU16 m_currentDrawFlags; // 0xc7a28
	undefined m_unk0xc7a2a[2];
	LegoFloat m_stageBlend;                                  // 0xc7a2c
	RendererAnimationResourceRecord m_animationResources[4]; // 0xc7a30
	LegoU32 m_unk0xc7ec0;
	LegoBool32 m_nvidiaWorkaround; // 0xc7ec4
	LegoU32 m_unk0xc7ec8;
	LegoU32 m_unk0xc7ecc;
	LegoU32 m_bufferStoragePolicy;                     // 0xc7ed0
	LegoU32 m_createDeviceBuffers;                     // 0xc7ed4
	LegoS32 m_streamResourceId;                        // 0xc7ed8
	LegoU32 m_vertexBufferCapacity;                    // 0xc7edc
	IDirect3DVertexBuffer8* m_normalVertexBuffer;      // 0xc7ee0, FVF 0x152
	IDirect3DVertexBuffer8* m_transformedVertexBuffer; // 0xc7ee4, FVF 0x144
	RendererIndexBuffer* m_streamingIndexBuffer;       // 0xc7ee8
	RendererVertexBuffer* m_quadVertexBuffer;          // 0xc7eec
	RendererIndexBuffer* m_quadIndexBuffer;            // 0xc7ef0
	LegoU32 m_streamVertexCount;                       // 0xc7ef4
	IDirect3DVertexBuffer8* m_specularVertexBuffer;    // 0xc7ef8, FVF 0x1c4
	RendererIndexBuffer* m_streamIndexBuffer;          // 0xc7efc
	BYTE* m_mappedStreamVertices;                      // 0xc7f00
	RendererDefaultRecord m_drawDefaults;              // 0xc7f04
};

// The storage is reserved without running the still-unfinished renderer
// constructor. Startup must construct it before invoking renderer methods.
DisplayRenderer* GetStaticDisplayRenderer();
extern DisplayRenderer* g_displayRenderer;

// This remains a prefix: the constructor proves storage through +0xc7f2b.
DECOMP_SIZE_ASSERT(DisplayRenderer, 0xc7f2c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_bucketedDraws) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_orderedDraws) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_deferredDraws) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureStageManager) == 0x25148);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureAddressModes) == 0x4fc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_stageAlpha) == 0x24544);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_stageFactorColor) == 0x2459c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureTransforms) == 0xc70a4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_stageBlend) == 0xc7a2c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureDefaults) == 0x28740);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_lights) == 0xc6290);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_lightCount) == 0xc7090);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_vertexRecords) == 0x28768);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_vertexStorage) == 0x8a1e8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_indexRecords) == 0x8b3e8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_indexStorage) == 0xc5d68);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_quadVertexBuffer) == 0xc7eec);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_streamIndexBuffer) == 0xc7efc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_drawDefaults) == 0xc7f04);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureRecords) == 0x38d8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_defaultTextureIndex) == 0x22cd8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureIndexMap) == 0x22cdc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_viewports) == 0x24360);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_currentViewportIndex) == 0x24524);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_viewportTransform) == 0x63c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_direct3D) == 0x7ac);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_clearQuadDepth) == 0x780);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureQuality) == 0x7ec);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureFormatCount) == 0x2395c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureFormats) == 0x23960);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_clearRenderTarget) == 0x24520);
DECOMP_SIZE_ASSERT(D3DMATERIAL8, 0x44);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_material) == 0x245a0);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_useBlendModeOverride) == 0xc6288);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_blendModeOverride) == 0xc628c);
DECOMP_SIZE_ASSERT(D3DPRESENT_PARAMETERS, 0x34);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_presentationParameters) == 0x7b8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_modeFormat) == 0x804);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedAdapter) == 0x820);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedTextureFormats) == 0x824);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedCompressedTextureFormats) == 0x844);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_adapterCount) == 0x38d4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_deviceBehaviorFlags) == 0x7f0);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_vertexBufferUsage) == 0x2456c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_foundPreferredDepthFormat) == 0x24570);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_ambientColor) == 0x24598);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_timingStatistics) == 0x245e8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_fillMode) == 0xc626c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_cachedRenderStates) == 0xc734c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_cachedTextureHandles) == 0xc75fc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_cachedTextureStageStates) == 0xc7624);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_nvidiaWorkaround) == 0xc7ec4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_vertexBufferCapacity) == 0xc7edc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_normalVertexBuffer) == 0xc7ee0);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_transformedVertexBuffer) == 0xc7ee4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_specularVertexBuffer) == 0xc7ef8);

void __stdcall MultiplyMatrix4(D3DMATRIX* p_result, const D3DMATRIX* p_left, const D3DMATRIX* p_right);

extern HRESULT g_lastDirect3DResult;

LegoU32 __stdcall GetDisplayFormatBitDepth(D3DFORMAT p_format);

#endif // LR2_RENDERER_H
