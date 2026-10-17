#include "matrix3.h"
#include "quaternion.h"
#include "renderer.h"
#include "rendererbufferdispatch.h"
#include "rendererdrawpacketview.h"
#include "renderertexturemanager.h"
#include "vector3.h"

#include <math.h>
#include <stddef.h>

extern HRESULT g_clearBuffersResult;
void __fastcall ApplySpecialDrawPacketEffect(
	DisplayRenderer* p_renderer,
	void* p_edx,
	RendererDrawPacketView* p_packet
);

// GLOBAL: LEGORACERS2 0x0067bd60
LegoFloat g_dotProductPacketAngle;

// GLOBAL: LEGORACERS2 0x0067bd64
LegoFloat g_textureAnimationPacketAngle;

namespace
{
typedef void(__fastcall* PacketSetValue)(DisplayRenderer*, void*, LegoU32);
typedef void(__fastcall* PacketAction)(DisplayRenderer*, void*);

struct RendererDrawPacketDispatch {
	undefined m_unk0x00[0xc0];
	PacketSetValue m_setColorOverride;
	PacketAction m_disableColorOverride;
	PacketSetValue m_setAlphaBlendMode;
	undefined m_unk0xcc[4];
	PacketAction m_clearBlendModeOverride;
};

DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketDispatch, m_setColorOverride) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketDispatch, m_setAlphaBlendMode) == 0xc8);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketDispatch, m_clearBlendModeOverride) == 0xd0);

const D3DPRIMITIVETYPE k_drawPacketPrimitiveTypes[6] = {
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_POINTLIST,
};

LegoU8 ScaleMaterialChannel(LegoFloat p_value)
{
	volatile LegoFloat stored = p_value * 255.0f;
	LegoS32 rounded;
	// The retail conversion is FISTP and follows the active x87 rounding mode.
	// clang-format off
	__asm {
		fld stored
		fistp rounded
	}
	// clang-format on
	return (LegoU8) rounded;
}

LegoU8 PackSignedDirectionChannel(LegoFloat p_value)
{
	volatile LegoFloat stored = p_value * 127.0f + 128.0f;
	LegoS32 rounded;
	// The retail conversion is FISTP and follows the active x87 rounding mode.
	// clang-format off
	__asm {
		fld stored
		fistp rounded
	}
	// clang-format on
	return (LegoU8) rounded;
}

RendererDefaultSlot* FindPacketDefaultSlot(RendererDefaultRecord* p_defaults, LegoS32 p_value)
{
	for (LegoS16 index = 0; index < (LegoS16) p_defaults->m_unk0x04; index++) {
		if ((LegoS32) p_defaults->m_slots[index].m_value == p_value) {
			return &p_defaults->m_slots[index];
		}
	}
	return NULL;
}

void MakeQuaternionFromUp(const Vector3* p_direction, Quaternion* p_result)
{
	// 0x004de990 supplies (sin(angle), 1, cos(angle)) after normalization,
	// then calls the quaternion-between-vectors helper with up=(0, 1, 0).
	// Its finite-input path has a positive dot and uses cross(up, direction),
	// normalized, times sin(half-angle); the scalar is cos(half-angle).
	Vector3 axis;
	axis.m_x = p_direction->m_z;
	axis.m_y = 0.0f;
	axis.m_z = -p_direction->m_x;
	axis.Normalize();

	const LegoFloat halfAngleSine = (LegoFloat) sqrt((1.0f - p_direction->m_y) * 0.5f);
	p_result->m_x = axis.m_x * halfAngleSine;
	p_result->m_y = axis.m_y * halfAngleSine;
	p_result->m_z = axis.m_z * halfAngleSine;
	p_result->m_w = (LegoFloat) sqrt((1.0f + p_direction->m_y) * 0.5f);
}

} // namespace

// STUB: LEGORACERS2 0x004dcf30
void DisplayRenderer::PrepareDrawPacketState(RendererDrawPacketView* p_packet)
{
	RendererAuxiliaryResourceRecord* resource = &m_auxiliaryResources[(LegoS32) p_packet->m_auxiliaryRecord];
	D3DMATERIAL8* material = (D3DMATERIAL8*) ((BYTE*) this + 0x24528);
	const RendererDrawPacketDispatch* dispatch = (const RendererDrawPacketDispatch*) m_vtable;

	if (m_textureTransformsEnabled == 1) {
		for (LegoU32 i = 0; i < 9; i++) {
			m_textureTransforms[i].m_used = 0;
		}
		m_textureTransformsEnabled = 0;
	}

	if (resource->m_callback != NULL) {
		typedef void(__cdecl * ResourceCallback)(RendererAuxiliaryResourceRecord*, LegoU32);
		((ResourceCallback) resource->m_callback)(resource, 0);
	}

	resource->CopyToDeviceMaterial(material);
	if (m_colorOverrideEnabled == 1) {
		material->Power = 0.0f;
		material->Diffuse.r = m_colorOverride[0];
		material->Diffuse.g = m_colorOverride[1];
		material->Diffuse.b = m_colorOverride[2];
		material->Diffuse.a = m_colorOverride[3];
		material->Ambient.r = m_colorOverride[0];
		material->Ambient.g = m_colorOverride[1];
		material->Ambient.b = m_colorOverride[2];
		material->Ambient.a = m_colorOverride[3];
		material->Emissive.r = m_colorOverride[0];
		material->Emissive.g = m_colorOverride[1];
		material->Emissive.b = m_colorOverride[2];
		material->Emissive.a = m_colorOverride[3];

		if (m_cachedRenderStates[0x94] != 0) {
			m_cachedRenderStates[0x94] = 0;
			m_device->SetRenderState((D3DRENDERSTATETYPE) 0x94, 0);
		}
		if (m_cachedRenderStates[0x8d] != 0) {
			m_cachedRenderStates[0x8d] = 0;
			m_device->SetRenderState((D3DRENDERSTATETYPE) 0x8d, 0);
		}
	}

	m_device->SetMaterial(material);
	LegoU32 specularEnable = material->Power == 0.0f ? 0 : m_unk0xc6270;
	if (m_cachedRenderStates[0x1d] != specularEnable) {
		m_cachedRenderStates[0x1d] = specularEnable;
		m_device->SetRenderState(D3DRS_SPECULARENABLE, specularEnable);
	}
	dispatch->m_setAlphaBlendMode(this, NULL, resource->m_mode);
}

// STUB: LEGORACERS2 0x004dd460
void DisplayRenderer::DrawRendererPacket(RendererDrawPacketView* p_packet)
{
	RendererVertexBuffer* vertices = p_packet->m_vertices;
	RendererIndexBuffer* indices = p_packet->m_indices;
	const RendererDrawPacketDispatch* dispatch = (const RendererDrawPacketDispatch*) m_vtable;
	const LegoU16 flags = p_packet->m_flags;
	IDirect3DIndexBuffer8* deviceIndices;

	m_currentDrawFlags = flags;
	if ((flags & 0x0100) != 0) {
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x88, 0);
	}
	if ((flags & 0xfeff) != 0) {
		if ((flags & 0x0020) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 1);
		}
		if ((flags & 0x0040) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 0);
		}
		if ((flags & 0x0008) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x17, 8);
		}
		if ((flags & 0x0010) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x0e, 0);
		}
		if ((flags & 0x0001) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x2f, 2);
		}
		if ((flags & 0x0200) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x94, 1);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x8d, 1);
		}
	}

	PrepareDrawPacketState(p_packet);
	RendererVertexStorage* vertexStorage = vertices->m_storage;
	const RendererVertexBufferDispatch* vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
	const RendererIndexBufferDispatch* indexDispatch = (const RendererIndexBufferDispatch*) indices->m_vtable;
	const LegoS32 baseVertex = vertices->m_baseVertex;
	const LegoU32 vertexFormat = vertexStorage->m_format;
	const LegoU32 stride = vertexDispatch->m_getStride(vertices, NULL);
	if (m_cachedStreamBuffer != vertexStorage->m_deviceBuffer) {
		m_cachedStreamBuffer = vertexStorage->m_deviceBuffer;
		m_device->SetStreamSource(0, vertexStorage->m_deviceBuffer, stride);
		if (m_cachedVertexShader != vertexFormat) {
			m_cachedVertexShader = vertexFormat;
			m_device->SetVertexShader(vertexFormat);
		}
	}
	deviceIndices = indexDispatch->m_getDeviceBuffer(indices, NULL);
	m_device->SetIndices(deviceIndices, baseVertex);

	if ((p_packet->m_flags & 0x0004) == 0) {
		if ((p_packet->m_defaults.m_unk0x00 & 8) != 0) {
			ApplySpecialDrawPacketEffect(this, NULL, p_packet);
		}
		RendererSubrecordGroup* group = m_textureStageManager.FindSubrecordGroup(p_packet->m_defaults.m_unk0x00);
		if (group != NULL) {
			for (LegoS32 index = 0; index < (LegoS32) group->m_unk0xc0; index++) {
				if (m_textureStageManager.ApplyStagePlanSubrecord(&p_packet->m_defaults, index) == 1) {
					g_clearBuffersResult = m_device->DrawIndexedPrimitive(
						k_drawPacketPrimitiveTypes[indices->m_unk0x0c],
						0,
						p_packet->m_vertexCount,
						0,
						p_packet->m_primitiveCount
					);
				}
			}
		}
	}

	if ((p_packet->m_flags & 0x0002) != 0) {
		RendererAuxiliaryResourceRecord* resource = &m_auxiliaryResources[p_packet->m_auxiliaryRecord];
		const LegoU32 color = (LegoU32) ScaleMaterialChannel(resource->m_scaledColor1[0]) |
							  ((LegoU32) ScaleMaterialChannel(resource->m_scaledColor1[1]) << 8) |
							  ((LegoU32) ScaleMaterialChannel(resource->m_scaledColor1[2]) << 16) |
							  ((LegoU32) ScaleMaterialChannel(1.0f - resource->m_modeFactor) << 24);
		dispatch->m_setColorOverride(this, NULL, color);
		SetCachedRenderState((D3DRENDERSTATETYPE) 8, 2);
		PrepareDrawPacketState(p_packet);
		BindMappedTexture(-1);
		g_clearBuffersResult = m_device->DrawIndexedPrimitive(
			k_drawPacketPrimitiveTypes[indices->m_unk0x0c],
			0,
			p_packet->m_vertexCount,
			0,
			p_packet->m_primitiveCount
		);
		dispatch->m_disableColorOverride(this, NULL);
		dispatch->m_clearBlendModeOverride(this, NULL);
		SetCachedRenderState((D3DRENDERSTATETYPE) 8, m_fillMode);
	}

	if (m_currentDrawFlags != 0) {
		const LegoU16 currentFlags = m_currentDrawFlags;
		if ((currentFlags & 0x0020) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3 - (m_clockwiseCulling != 0));
		}
		if ((currentFlags & 0x0040) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 1);
		}
		if ((currentFlags & 0x0008) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x17, 4);
		}
		if ((currentFlags & 0x0010) != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x0e, 1);
		}
		if (((currentFlags & 0x0001) != 0) && m_cachedRenderStates[0x2f] != 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x2f, 0);
		}
		if ((currentFlags & 0x0200) != 0) {
			if (m_cachedRenderStates[0x94] != 0) {
				SetCachedRenderState((D3DRENDERSTATETYPE) 0x94, 0);
			}
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x8d, 0);
		}
	}
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x88, 1);
}

// STUB: LEGORACERS2 0x004dd900
void DisplayRenderer::DrawIndexedPacket(RendererDrawPacketView* p_packet)
{
	RendererVertexBuffer* vertices = p_packet->m_vertices;
	RendererVertexStorage* storage = vertices->m_storage;
	IDirect3DVertexBuffer8* deviceVertices = storage->m_deviceBuffer;
	LegoS32 baseVertex = vertices->m_baseVertex;
	const RendererVertexBufferDispatch* vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
	LegoU32 stride = vertexDispatch->m_getStride(vertices, NULL);
	LegoU32 vertexFormat = storage->m_format;
	RendererIndexBuffer* indexBuffer = p_packet->m_indices;
	const RendererIndexBufferDispatch* indexDispatch = (const RendererIndexBufferDispatch*) indexBuffer->m_vtable;

	if (m_cachedStreamBuffer != deviceVertices) {
		m_cachedStreamBuffer = deviceVertices;
		m_device->SetStreamSource(0, deviceVertices, stride);
		if (m_cachedVertexShader != vertexFormat) {
			m_cachedVertexShader = vertexFormat;
			m_device->SetVertexShader(vertexFormat);
		}
	}
	IDirect3DIndexBuffer8* indices = indexDispatch->m_getDeviceBuffer(indexBuffer, NULL);
	m_device->SetIndices(indices, baseVertex);

	g_clearBuffersResult = m_device->DrawIndexedPrimitive(
		k_drawPacketPrimitiveTypes[indexBuffer->m_unk0x0c],
		0,
		p_packet->m_vertexCount,
		0,
		p_packet->m_primitiveCount
	);
}

// STUB: LEGORACERS2 0x004de990
void __fastcall ApplySpecialDrawPacketEffect(DisplayRenderer* p_renderer, void*, RendererDrawPacketView* p_packet)
{
	RendererVertexBuffer* vertices = p_packet->m_vertices;
	const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
	Matrix3 world;
	world.m_values[0][0] = p_renderer->m_worldTransform.m[0][0];
	world.m_values[0][1] = p_renderer->m_worldTransform.m[1][0];
	world.m_values[0][2] = p_renderer->m_worldTransform.m[2][0];
	world.m_values[1][0] = p_renderer->m_worldTransform.m[0][1];
	world.m_values[1][1] = p_renderer->m_worldTransform.m[1][1];
	world.m_values[1][2] = p_renderer->m_worldTransform.m[2][1];
	world.m_values[2][0] = p_renderer->m_worldTransform.m[0][2];
	world.m_values[2][1] = p_renderer->m_worldTransform.m[1][2];
	world.m_values[2][2] = p_renderer->m_worldTransform.m[2][2];

	if (p_renderer->SupportsDotProduct3() == 1) {
		const LegoS32 vertexCount = dispatch->m_getCount(vertices, NULL);
		Vector3 direction;
		direction.m_x = (LegoFloat) sin(g_dotProductPacketAngle);
		direction.m_y = 1.0f;
		direction.m_z = (LegoFloat) cos(g_dotProductPacketAngle);
		direction.Normalize();

		Quaternion quaternion;
		MakeQuaternionFromUp(&direction, &quaternion);
		Matrix3 rotation;
		rotation.FUN_004a4d30(&quaternion.m_x);
		Matrix3 combined;
		combined.FUN_004a4ea0(&rotation, &world);
		g_dotProductPacketAngle += 0.02f;

		dispatch->m_setBufferPolicy(vertices, NULL, 1);
		if (dispatch->m_lock(vertices, NULL, 0, vertexCount, 0, 0) == 1) {
			dispatch->m_setCursor(vertices, NULL, 0);
			for (LegoS32 index = 0; index < vertexCount; index++) {
				Vector3* position = dispatch->m_getCurrentPosition(vertices, NULL);
				Vector3 transformed = *position;
				transformed.FUN_004a4e00(&combined);

				LegoU32 color = (LegoU32) p_packet & 0xff000000;
				color |= (LegoU32) PackSignedDirectionChannel(transformed.m_y);
				color |= (LegoU32) PackSignedDirectionChannel(transformed.m_z) << 8;
				color |= (LegoU32) PackSignedDirectionChannel(transformed.m_x) << 16;
				dispatch->m_setCurrentColor(vertices, NULL, &color);
				dispatch->m_advanceCursor(vertices, NULL);
			}
			dispatch->m_unlock(vertices, NULL, 1);
		}
		dispatch->m_setBufferPolicy(vertices, NULL, 0);
		return;
	}

	RendererDefaultRecord* defaults = &p_packet->m_defaults;
	RendererDefaultSlot* coordinateSource = FindPacketDefaultSlot(defaults, 0);
	RendererDefaultSlot* textureSlot = FindPacketDefaultSlot(defaults, 3);
	if (coordinateSource == NULL || textureSlot == NULL || p_packet->m_payload == NULL) {
		return;
	}

	const LegoU32 sourceStage = coordinateSource->m_kind & 0x0f;
	const LegoU32 targetStage = textureSlot->m_kind & 0x0f;
	const LegoS16 textureMapIndex = (LegoS16) textureSlot->m_mask;
	const LegoU32 textureRecordIndex = p_renderer->m_textureIndexMap[textureMapIndex];
	const RendererTextureRecord* texture = &p_renderer->m_textureRecords[textureRecordIndex];
	const LegoFloat inverseWidth = 1.0f / (LegoFloat) texture->m_width;
	const LegoFloat inverseHeight = 1.0f / (LegoFloat) texture->m_height;

	Vector3 direction;
	direction.m_x = (LegoFloat) sin(g_textureAnimationPacketAngle);
	direction.m_y = 1.0f;
	direction.m_z = (LegoFloat) cos(g_textureAnimationPacketAngle);
	direction.Normalize();
	direction.FUN_004a4e00(&world);
	g_textureAnimationPacketAngle += 0.002f;

	if (dispatch->m_lock(vertices, NULL, 0, p_packet->m_vertexCount, 0, 0) == 1) {
		dispatch->m_setCursor(vertices, NULL, 0);
		const LegoFloat* input = (const LegoFloat*) p_packet->m_payload;
		for (LegoU32 index = 0; index < p_packet->m_vertexCount; index++) {
			const LegoFloat rotatedU = direction.m_x * input[0] + direction.m_y * input[1] + direction.m_z * input[2];
			const LegoFloat rotatedV = direction.m_x * input[3] + direction.m_y * input[4] + direction.m_z * input[5];
			RendererTextureCoordinate* base = dispatch->m_getCurrentTextureCoordinate(vertices, NULL, sourceStage);
			RendererTextureCoordinate output;
			output.m_u = rotatedU * inverseWidth + base->m_u;
			output.m_v = rotatedV * inverseHeight + base->m_v;
			dispatch->m_setCurrentTextureCoordinate(vertices, NULL, targetStage, &output);
			dispatch->m_advanceCursor(vertices, NULL);
			input += 6;
		}
		dispatch->m_unlock(vertices, NULL, 1);
	}
}
