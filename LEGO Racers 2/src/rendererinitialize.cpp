#include "application.h"
#include "bitmapimage.h"
#include "commandline.h"
#include "displayconfig.h"
#include "memoryheap.h"
#include "nativewindow.h"
#include "platformfpu.h"
#include "renderer.h"
#include "rendererdrawpacketview.h"
#include "startup.h"

#include <string.h>

extern HRESULT g_lastDirect3DResult;
LegoU32 __stdcall GetDisplayFormatBitDepth(D3DFORMAT p_format);

namespace
{
// These renderer-table signatures follow the stack arguments at the call
// sites in retail 0x004ac130. The table is not emitted as a complete source
// class until its other slots are recovered.
typedef void(__fastcall* InitializeTexture)(
	DisplayRenderer* p_renderer,
	void* p_edx,
	LegoU32 p_default,
	LegoU32 p_record,
	LegoU32 p_width,
	LegoU32 p_height,
	LegoU32 p_flags,
	LegoBool32* p_initialized,
	LegoU32 p_ignored
);

void InitializeRendererTexture(DisplayRenderer* p_renderer, LegoU32 p_record)
{
	void** table = (void**) p_renderer->m_vtable;
	InitializeTexture initialize = (InitializeTexture) table[0xd4 / sizeof(void*)];
	LegoBool32 initialized;
	initialize(p_renderer, NULL, 1, p_record, 4, 4, 1, &initialized, 0);
}

typedef void(__fastcall* ReleaseObject)(void* p_object, void* p_edx);

struct RendererDrawPacket {
	LegoU16 m_tag0;
	LegoU16 m_tag1;
	LegoU16 m_tag2;
	LegoU16 m_tag3;
	void* m_heapData;
	void* m_releaseObject0;
	void* m_releaseObject1;
	undefined m_defaults[0x28];
};
DECOMP_SIZE_ASSERT(RendererDrawPacket, 0x3c);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacket, m_heapData) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacket, m_defaults) == 0x14);
} // namespace

// STUB: LEGORACERS2 0x004a7cc0
void __fastcall ReleaseRendererDrawPacket(RendererDrawPacketView* p_packet, void*)
{
	if (p_packet->m_payload != NULL) {
		GetMemoryHeap()->Free(p_packet->m_payload);
	}
	if (p_packet->m_indices != NULL) {
		void** table = *(void***) p_packet->m_indices;
		ReleaseObject release = (ReleaseObject) table[0];
		release(p_packet->m_indices, NULL);
	}
	if (p_packet->m_vertices != NULL) {
		void** table = *(void***) p_packet->m_vertices;
		ReleaseObject release = (ReleaseObject) table[0];
		release(p_packet->m_vertices, NULL);
	}
}

// STUB: LEGORACERS2 0x004ac130
LegoBool32 DisplayRenderer::Initialize()
{
	Application* application = GetApplication();
	ApplicationDisplaySettings* displaySettings = application->GetDisplaySettings();
	LegoS32 requestedWidth = displaySettings->m_width;
	LegoS32 requestedHeight = displaySettings->m_height;
	LegoS32 requestedBitDepth = displaySettings->m_bitDepth;
	LegoS32 requestedRefreshRate = 60;
	LegoS32 windowed;
	MEMORYSTATUS memoryStatus;

	m_requestedAdapterIndex = -1;
	memset(&memoryStatus, 0, sizeof(memoryStatus));
	memoryStatus.dwLength = sizeof(memoryStatus);
	GlobalMemoryStatus(&memoryStatus);
	m_totalPhysicalMemory = memoryStatus.dwTotalPhys;

	g_displayConfiguration = (DisplayConfiguration*) AllocateHeapMemory(sizeof(DisplayConfiguration));
	if (g_displayConfiguration != NULL) {
		g_displayConfiguration->m_unk0x10 = 0;
		g_displayConfiguration->m_dithering = 1;
		g_displayConfiguration->m_antiAliasing = 0;
		g_displayConfiguration->m_writeOnlyVBs = 1;
		g_displayConfiguration->m_largeVBs = 1;
		g_displayConfiguration->m_hardwareTransformAndLighting = 1;
		g_displayConfiguration->m_nvidia = 0;
		g_displayConfiguration->m_vSync = 1;
		g_displayConfiguration->m_tripleBuffer = 0;
		g_displayConfiguration->m_unk0x38 = 0;
	}

	strcpy(g_windowTitle, application->m_vtable->m_title(application, NULL));
	if (FUN_004b4a60("/width")) {
		requestedWidth = FUN_004b4b30("/width");
	}
	if (FUN_004b4a60("/height")) {
		requestedHeight = FUN_004b4b30("/height");
	}
	if (FUN_004b4a60("/bpp")) {
		requestedBitDepth = FUN_004b4b30("/bpp");
	}
	if (FUN_004b4a60("/hz")) {
		requestedRefreshRate = FUN_004b4b30("/hz");
	}
	if (FUN_004b4a60("/device")) {
		m_requestedAdapterIndex = FUN_004b4b30("/device");
	}
	if (FUN_004b4a60("/wireframe")) {
		m_fillMode = 2;
	}

	windowed = application->IsDevelopmentMode() != 0 ? 1 : FUN_004b4a60("/windowed");
	g_displayConfiguration->m_width = requestedWidth;
	g_displayConfiguration->m_height = requestedHeight;
	g_displayConfiguration->m_bitDepth = requestedBitDepth;
	g_displayConfiguration->m_fullscreen = windowed == 0;
	g_displayConfiguration->m_heightToWidthRatio = (LegoFloat) requestedHeight / requestedWidth;
	g_displayConfiguration->m_unk0x38 = m_requestedAdapterIndex;
	if (FUN_004b4a60("/options")) {
		application->m_vtable->m_settingsDialog(application, NULL);
	}
	m_vertexBufferUsage = g_displayConfiguration->m_writeOnlyVBs == 1 ? 8 : 0;

	CreateNativeWindow(g_startupWndProc);
	if (windowed == 0) {
		ShowCursor(FALSE);
	}
	SaveFloatingPointControl();
	InitializeRendererFloatingPoint();
	m_direct3D = Direct3DCreate8(D3D_SDK_VERSION);
	if (m_direct3D == NULL || !EnumerateSupportedAdapters() || m_adapterCount < 1) {
		return FALSE;
	}

	LegoS32 preferredAdapter = 0;
	while (preferredAdapter < (LegoS32) m_adapterCount &&
		   (m_adapters[preferredAdapter].m_caps.DevCaps & 0x10000) == 0) {
		preferredAdapter++;
	}
	if (preferredAdapter == (LegoS32) m_adapterCount) {
		preferredAdapter = 0;
	}
	if (m_requestedAdapterIndex != -1 && m_requestedAdapterIndex < (LegoS32) m_adapterCount) {
		preferredAdapter = m_requestedAdapterIndex;
	}
	m_requestedAdapterIndex = preferredAdapter;
	m_selectedAdapter = &m_adapters[preferredAdapter];

	if (windowed == 1) {
		D3DDISPLAYMODE desktopMode;
		memset(&desktopMode, 0, sizeof(desktopMode));
		m_direct3D->GetAdapterDisplayMode(m_selectedAdapter->m_adapterIndex, &desktopMode);
		requestedWidth = desktopMode.Width;
		requestedHeight = desktopMode.Height;
		requestedBitDepth = GetDisplayFormatBitDepth(desktopMode.Format);
		requestedRefreshRate = desktopMode.RefreshRate;
		FindBestDisplayMode(
			(LegoU32*) &requestedWidth,
			(LegoU32*) &requestedHeight,
			(LegoU32*) &requestedBitDepth,
			(LegoU32*) &requestedRefreshRate
		);
		RECT clientRect;
		GetClientRect(g_viewWindow, &clientRect);
		m_modeWidth = clientRect.right;
		m_modeHeight = clientRect.bottom;
		if (application->IsDevelopmentMode() == 0) {
			g_displayConfiguration->m_width = clientRect.right;
			g_displayConfiguration->m_height = clientRect.bottom;
			g_displayConfiguration->m_bitDepth = requestedBitDepth;
			g_displayConfiguration->m_fullscreen = 0;
			g_displayConfiguration->m_heightToWidthRatio =
				(LegoFloat) (LegoS32) clientRect.bottom / (LegoS32) clientRect.right;
		}
		m_selectedAdapter->m_adapterIndex = 0;
	}
	else if (!FindBestDisplayMode(
				 (LegoU32*) &requestedWidth,
				 (LegoU32*) &requestedHeight,
				 (LegoU32*) &requestedBitDepth,
				 (LegoU32*) &requestedRefreshRate
			 )) {
		requestedRefreshRate = 0;
		if (!FindBestDisplayMode(
				(LegoU32*) &requestedWidth,
				(LegoU32*) &requestedHeight,
				(LegoU32*) &requestedBitDepth,
				(LegoU32*) &requestedRefreshRate
			)) {
			requestedBitDepth = 16;
			if (!FindBestDisplayMode(
					(LegoU32*) &requestedWidth,
					(LegoU32*) &requestedHeight,
					(LegoU32*) &requestedBitDepth,
					(LegoU32*) &requestedRefreshRate
				)) {
				return FALSE;
			}
		}
	}

	if ((m_selectedAdapter->m_caps.DevCaps & 0x10000) != 0 &&
		g_displayConfiguration->m_hardwareTransformAndLighting == 1) {
		m_deviceBehaviorFlags = (m_selectedAdapter->m_caps.DevCaps & 0x100000) != 0 ? 0x50 : 0x40;
	}
	else {
		m_deviceBehaviorFlags = 0x20;
	}
	if (application->IsDevelopmentMode() != 0) {
		m_deviceBehaviorFlags |= 4;
	}

	memset(&m_presentationParameters, 0, sizeof(m_presentationParameters));
	m_presentationParameters.Windowed = windowed;
	m_presentationParameters.EnableAutoDepthStencil = TRUE;
	m_presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_presentationParameters.hDeviceWindow = g_mainWindow;
	m_presentationParameters.BackBufferWidth = m_modeWidth;
	m_presentationParameters.BackBufferHeight = m_modeHeight;
	m_presentationParameters.BackBufferFormat = m_modeFormat;
	m_presentationParameters.MultiSampleType = (D3DMULTISAMPLE_TYPE) 0;
	SelectDepthStencilFormat();

	if (windowed == 1) {
		m_presentationParameters.BackBufferCount = 1;
		m_presentationParameters.FullScreen_PresentationInterval = 0;
		g_lastDirect3DResult = m_direct3D->CreateDevice(
			m_selectedAdapter->m_adapterIndex,
			D3DDEVTYPE_HAL,
			g_mainWindow,
			m_deviceBehaviorFlags,
			&m_presentationParameters,
			&m_device
		);
		if (g_lastDirect3DResult != D3D_OK) {
			return FALSE;
		}
	}
	else {
		if (g_displayConfiguration->m_antiAliasing == 1) {
			const D3DMULTISAMPLE_TYPE sampleTypes[] =
				{D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_3_SAMPLES, D3DMULTISAMPLE_4_SAMPLES};
			for (LegoU32 i = 0; i < sizeOfArray(sampleTypes); ++i) {
				g_lastDirect3DResult = m_direct3D->CheckDeviceMultiSampleType(
					m_selectedAdapter->m_adapterIndex,
					D3DDEVTYPE_HAL,
					m_modeFormat,
					FALSE,
					sampleTypes[i]
				);
				if (g_lastDirect3DResult == D3D_OK) {
					m_presentationParameters.MultiSampleType = sampleTypes[i];
					break;
				}
			}
		}
		m_presentationParameters.FullScreen_RefreshRateInHz = m_modeRefreshRate;
		m_presentationParameters.FullScreen_PresentationInterval =
			g_displayConfiguration->m_vSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		m_presentationParameters.BackBufferCount =
			g_displayConfiguration->m_tripleBuffer == 1 && g_displayConfiguration->m_vSync == 1 ? 2 : 1;
		g_lastDirect3DResult = m_direct3D->CreateDevice(
			m_selectedAdapter->m_adapterIndex,
			D3DDEVTYPE_HAL,
			g_mainWindow,
			m_deviceBehaviorFlags,
			&m_presentationParameters,
			&m_device
		);
		if (g_lastDirect3DResult != D3D_OK) {
			requestedWidth = 640;
			requestedHeight = 480;
			requestedBitDepth = 16;
			requestedRefreshRate = 0;
			if (!FindBestDisplayMode(
					(LegoU32*) &requestedWidth,
					(LegoU32*) &requestedHeight,
					(LegoU32*) &requestedBitDepth,
					(LegoU32*) &requestedRefreshRate
				)) {
				return FALSE;
			}
			m_presentationParameters.BackBufferWidth = m_modeWidth;
			m_presentationParameters.BackBufferHeight = m_modeHeight;
			m_presentationParameters.BackBufferFormat = m_modeFormat;
			g_lastDirect3DResult = m_direct3D->CreateDevice(
				m_selectedAdapter->m_adapterIndex,
				D3DDEVTYPE_HAL,
				g_mainWindow,
				m_deviceBehaviorFlags,
				&m_presentationParameters,
				&m_device
			);
			if (g_lastDirect3DResult != D3D_OK) {
				return FALSE;
			}
			g_displayConfiguration->m_width = m_modeWidth;
			g_displayConfiguration->m_height = m_modeHeight;
			g_displayConfiguration->m_bitDepth = requestedBitDepth;
			g_displayConfiguration->m_fullscreen = 1;
			g_displayConfiguration->m_heightToWidthRatio = (LegoFloat) (LegoS32) m_modeHeight / (LegoS32) m_modeWidth;
		}
	}

	m_deviceMemory = m_device->GetAvailableTextureMem();
	memset(&m_defaultWorldTransform, 0, sizeof(m_defaultWorldTransform));
	m_defaultWorldTransform.m[0][0] = 1.0f;
	m_defaultWorldTransform.m[1][1] = 1.0f;
	m_defaultWorldTransform.m[2][2] = 1.0f;
	m_defaultWorldTransform.m[3][3] = 1.0f;
	ConfigureNvidiaWorkaround();
	if (!InitializeTextureFormats()) {
		return FALSE;
	}

	LegoS32 renderTarget = m_vtable->m_addViewport(this, NULL, 0, 0, m_modeWidth, m_modeHeight);
	m_clearRenderTarget = renderTarget;
	if (renderTarget == -1) {
		return FALSE;
	}
	m_vtable->m_setRenderTarget(this, NULL, renderTarget);
	InitializeDefaultRenderStates();
	m_vtable->m_setMaterialColor(this, NULL, 0);
	m_textureStageManager.Initialize();
	InitializeDefaultRenderStates();
	m_device->SetRenderState((D3DRENDERSTATETYPE) 0x1f, 1);
	m_device->SetRenderState((D3DRENDERSTATETYPE) 0x1f, 0);
	for (LegoS32 i = 0; i < 4; ++i) {
		m_vtable->m_clearBuffers(this, NULL, 7);
		m_vtable->m_present(this, NULL);
	}

	LegoU32 textureId = CreateTextureResource("", 0, -1, 0, 1);
	m_defaultTextureIndex = textureId;
	InitializeRendererTexture(this, textureId);

	undefined bitmapStorage[sizeof(BitmapImage)];
	memset(bitmapStorage, 0, sizeof(bitmapStorage));
	BitmapImage* bitmapImage = (BitmapImage*) bitmapStorage;
	bitmapImage->m_width = 4;
	bitmapImage->m_height = 4;
	bitmapImage->m_bitsPerPixel = 32;
	bitmapImage->m_mipLevels = 0;
	bitmapImage->m_unk0x10 = 1;
	LegoU32 whitePixels[16];
	for (LegoU32 pixelIndex = 0; pixelIndex < sizeOfArray(whitePixels); ++pixelIndex) {
		whitePixels[pixelIndex] = 0xffffffff;
	}
	bitmapImage->m_pixels = (LegoU8*) whitePixels;
	RendererTextureRecord* textureRecord = m_vtable->m_getTextureRecord(this, NULL, (LegoS32) textureId);
	typedef void(__fastcall * ApplyTextureDescription)(RendererTextureRecord*, void*, const BitmapImage*);
	void** textureRecordTable = (void**) textureRecord->m_vtable;
	ApplyTextureDescription applyTextureDescription =
		(ApplyTextureDescription) textureRecordTable[0x24 / sizeof(void*)];
	applyTextureDescription(textureRecord, NULL, bitmapImage);

	void** rendererTable = (void**) m_vtable;
	typedef RendererVertexBuffer*(__fastcall * CreateVertexBuffer)(
		DisplayRenderer*,
		void*,
		LegoU32,
		LegoS32,
		LegoS32,
		LegoU32
	);
	CreateVertexBuffer createVertexBuffer = (CreateVertexBuffer) rendererTable[0x44 / sizeof(void*)];
	m_quadVertexBuffer = createVertexBuffer(this, NULL, 1, 12000, 0, 1);
	typedef void(__fastcall * SetVertexBufferPolicy)(RendererVertexBuffer*, void*, LegoU32);
	void** vertexTable = (void**) m_quadVertexBuffer->m_vtable;
	SetVertexBufferPolicy setVertexBufferPolicy = (SetVertexBufferPolicy) vertexTable[0x1c / sizeof(void*)];
	setVertexBufferPolicy(m_quadVertexBuffer, NULL, 1);

	m_quadIndexBuffer = m_vtable->m_createIndexBuffer(this, NULL, 0, 18000);
	if (m_quadIndexBuffer->Lock(0, 18000, 1) == TRUE) {
		LegoU16* indices = (LegoU16*) m_quadIndexBuffer->m_data;
		for (LegoS32 quadIndex = 0; quadIndex < 3000; ++quadIndex) {
			LegoS32 vertex = quadIndex * 4;
			indices[quadIndex * 6 + 0] = (LegoU16) vertex;
			indices[quadIndex * 6 + 1] = (LegoU16) (vertex + 1);
			indices[quadIndex * 6 + 2] = (LegoU16) (vertex + 2);
			indices[quadIndex * 6 + 3] = (LegoU16) (vertex + 1);
			indices[quadIndex * 6 + 4] = (LegoU16) (vertex + 3);
			indices[quadIndex * 6 + 5] = (LegoU16) (vertex + 2);
		}
		m_quadIndexBuffer->Unlock(1, 1);
	}

	m_streamIndexBuffer = m_vtable->m_createIndexBuffer(this, NULL, 0, 0xc00);
	if (m_streamIndexBuffer->Lock(0, 0xc00, 1) == TRUE) {
		LegoU16* indices = (LegoU16*) m_streamIndexBuffer->m_data;
		for (LegoS32 quadIndex = 0; quadIndex < 0x200; ++quadIndex) {
			LegoS32 vertex = quadIndex * 4;
			indices[quadIndex * 6 + 0] = (LegoU16) vertex;
			indices[quadIndex * 6 + 1] = (LegoU16) (vertex + 1);
			indices[quadIndex * 6 + 2] = (LegoU16) (vertex + 2);
			indices[quadIndex * 6 + 3] = (LegoU16) (vertex + 1);
			indices[quadIndex * 6 + 4] = (LegoU16) (vertex + 3);
			indices[quadIndex * 6 + 5] = (LegoU16) (vertex + 2);
		}
		m_streamIndexBuffer->Unlock(1, 1);
	}

	ConfigureStreamResources(1);
	m_drawDefaults.m_unk0x00 = 0;
	m_drawDefaults.m_unk0x04 = 0;
	m_drawDefaults.m_unk0x06 = 0;
	m_drawDefaults.m_unk0x07 = 2;
	m_drawDefaults.m_slots[0].m_value = 0;
	m_drawDefaults.m_slots[0].m_mask = 0xffff;
	m_drawDefaults.m_slots[0].m_kind = 0;
	m_drawDefaults.m_slots[0].m_flags = 0;
	m_drawDefaults.m_unk0x04 = 1;
	m_drawDefaults.m_unk0x00 |= 1;
	m_drawDefaults.m_slots[1].m_value = 4;
	m_drawDefaults.m_slots[1].m_mask = 0xffff;
	m_drawDefaults.m_slots[1].m_kind = 1;
	m_drawDefaults.m_slots[1].m_flags = 0;
	m_drawDefaults.m_unk0x04 = 2;
	m_drawDefaults.m_unk0x00 |= 0x10;

	RendererDrawPacket packet;
	memset(&packet, 0, sizeof(packet));
	RendererDefaultRecord* defaults = (RendererDefaultRecord*) packet.m_defaults;
	defaults->m_unk0x00 = 1;
	defaults->m_unk0x04 = 1;
	defaults->m_unk0x06 = 0;
	defaults->m_unk0x07 = 1;
	for (LegoS32 slotIndex = 0; slotIndex < 4; ++slotIndex) {
		defaults->m_slots[slotIndex].m_value = 0xffffffff;
		defaults->m_slots[slotIndex].m_mask = 0xffff;
		defaults->m_slots[slotIndex].m_kind = 0x0f;
		defaults->m_slots[slotIndex].m_flags = 3;
	}
	defaults->m_slots[0].m_value = 0;
	defaults->m_slots[0].m_mask = 0;
	defaults->m_slots[0].m_kind = 0;
	defaults->m_slots[0].m_flags = 0;
	typedef void(__fastcall * InitializeRendererRecord)(DisplayRenderer*, void*, const void*);
	InitializeRendererRecord initializeRendererRecord = (InitializeRendererRecord) rendererTable[0x80 / sizeof(void*)];
	initializeRendererRecord(this, NULL, &packet);
	if (m_createDeviceBuffers == 1) {
		CreateVertexBuffers();
	}
	ReleaseRendererDrawPacket((RendererDrawPacketView*) &packet, NULL);
	return TRUE;
}
