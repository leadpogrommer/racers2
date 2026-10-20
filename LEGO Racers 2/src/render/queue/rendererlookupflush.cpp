#include "math/matrix3.h"
#include "math/vector3.h"
#include "render/buffers/rendererbufferdispatch.h"
#include "render/buffers/rendererbufferobjects.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/terrain/rendererterrain.h"

#include <string.h>

extern DisplayRenderer* g_displayRenderer;

extern void __fastcall RenderDeferredResource(DisplayRenderer* p_renderer, void*, void* p_resource);
extern void __fastcall RenderInitializedResource(void* p_object, void*, void* p_resource);

// The next independently initialized renderer array begins at 0x0067b638.
// This address range is used as point-depth scratch; the 1000-float extent is
// only a working storage bound and is not asserted as the original capacity.
// GLOBAL: LEGORACERS2 0x0067a698
LegoFloat g_rendererPointDepthScratch[1000] = {0.0f};

struct RendererStencilMeshView {
	void* m_groupData;
	const Vector3* m_points;
	LegoS32 m_groupCount;
	LegoS32 m_pointCount;
	LegoU32 m_enabled;
	LegoFloat m_halfWidth;
	LegoFloat m_halfHeight;
	LegoFloat m_centerX;
	LegoFloat m_depthOffset;
	LegoFloat m_centerZ;
};

static LegoBool32 IsStencilPointInFront(LegoS16 p_pointIndex)
{
	const LegoU8* depth = (const LegoU8*) g_rendererPointDepthScratch + (LegoS32) p_pointIndex * 4;
	return (depth[3] & 0x80) == 0;
}

static void CopyStencilPoint(LegoU8* p_destination, const LegoFloat* p_point)
{
	memcpy(p_destination, p_point, sizeof(LegoFloat) * 3);
}

// STUB: LEGORACERS2 0x004bd870
void RendererLookupTableState::FlushOrderedEntries()
{
	if (m_itemCount <= 0) {
		return;
	}

	for (LegoS32 bucket = (LegoS32) m_bucketCount - 1; bucket >= 0; --bucket) {
		Entry* entry = m_bucketHeads[bucket];
		while (entry != NULL) {
			g_displayRenderer->DrawLookupEntry(entry);
			entry = entry->m_next;
		}
	}
}

// STUB: LEGORACERS2 0x004bd8c0
void RendererLookupTableState::FlushBucketedEntries()
{
	if (m_itemCount <= 0) {
		return;
	}

	Entry** tail = m_bucketTails;
	while (*tail != NULL) {
		Entry* entry = *tail;
		while (entry != NULL) {
			g_displayRenderer->DrawLookupEntry(entry);
			entry = entry->m_next;
		}
		++tail;
	}

	Clear();
}

// STUB: LEGORACERS2 0x004bd900
void RendererLookupTableState::FlushDeferredDrawResources()
{
	if (g_displayRenderer->m_unk0x4d0 < 3 || m_itemCount <= 0) {
		return;
	}

	RendererMainDispatch* dispatch = (RendererMainDispatch*) g_displayRenderer->m_vtable;
	if (dispatch->m_supportsExtendedTextureStages(g_displayRenderer, NULL) != 1) {
		Entry** bucket = m_bucketTails;
		while (*bucket != NULL) {
			Entry* entry = *bucket;
			while (entry != NULL) {
				RenderDeferredResource(g_displayRenderer, NULL, (void*) entry->m_unk0x0c);
				entry = entry->m_next;
			}
			++bucket;
		}
		return;
	}

	Entry** bucket = m_bucketTails;
	while (*bucket != NULL) {
		Entry* entry = *bucket;
		while (entry != NULL) {
			void* resource = (void*) entry->m_unk0x0c;
			void* object = *(void**) ((LegoU8*) resource + 8);
			void* initialized = *(void**) ((LegoU8*) object + 0x28);
			if (initialized == NULL || *(LegoU32*) ((LegoU8*) initialized + 4) == 0) {
				RenderDeferredResource(g_displayRenderer, NULL, resource);
			}
			entry = entry->m_next;
		}
		++bucket;
	}

	dispatch->m_applyTextureOperation(g_displayRenderer, NULL, 3);

	bucket = m_bucketTails;
	while (*bucket != NULL) {
		Entry* entry = *bucket;
		while (entry != NULL) {
			void* resource = (void*) entry->m_unk0x0c;
			void* object = *(void**) ((LegoU8*) resource + 8);
			void* initialized = *(void**) ((LegoU8*) object + 0x28);
			if (initialized != NULL && *(LegoU32*) ((LegoU8*) initialized + 4) != 0) {
				RenderInitializedResource(object, NULL, resource);
			}
			entry = entry->m_next;
		}
		++bucket;
	}

	dispatch->m_drawStencilOverlay(g_displayRenderer, NULL);
	dispatch->m_applyTextureOperation(g_displayRenderer, NULL, 4);
}

// FUNCTION length: 0x305 bytes (0x004c6980-0x004c6c84).
// STUB: LEGORACERS2 0x004c6980
void __fastcall RenderInitializedResource(void* p_object, void*, void* p_resource)
{
	DisplayRenderer* renderer = g_displayRenderer;
	LegoU8* resource = (LegoU8*) p_resource;
	RendererStencilMeshView* mesh = *(RendererStencilMeshView**) ((LegoU8*) p_object + 0x28);
	RendererMainDispatch* rendererDispatch = (RendererMainDispatch*) renderer->m_vtable;
	if (rendererDispatch->m_supportsExtendedTextureStages(renderer, NULL) == 0 || mesh == NULL ||
		mesh->m_enabled == 0) {
		return;
	}

	Matrix3 rotation;
	rotation.FUN_004a4d30((const LegoFloat*) (resource + 0x8c));
	Matrix3 inverseRotation;
	inverseRotation.FUN_004a5040(&rotation);
	Vector3 depthDirection = *(Vector3*) (resource + 0xd8);
	depthDirection.FUN_004a4e00(&inverseRotation);
	Vector3 extrusion = depthDirection;
	extrusion.m_x *= 50.0f;
	extrusion.m_y *= 50.0f;
	extrusion.m_z *= 50.0f;

	for (LegoS32 pointIndex = 0; pointIndex < mesh->m_pointCount; ++pointIndex) {
		const Vector3* point = &mesh->m_points[pointIndex];
		g_rendererPointDepthScratch[pointIndex] =
			depthDirection.m_x * point->m_x + depthDirection.m_y * point->m_y + depthDirection.m_z * point->m_z;
	}

	RendererVertexBuffer* vertexBuffer = rendererDispatch->m_getQuadVertexBuffer(renderer, NULL);
	if (vertexBuffer == NULL) {
		return;
	}
	RendererVertexBufferDispatch* vertexDispatch = (RendererVertexBufferDispatch*) vertexBuffer->m_vtable;
	if (vertexDispatch->m_lock(vertexBuffer, NULL, 0, 0, 1, 1) != 1) {
		return;
	}

	LegoU8* destination =
		vertexDispatch->m_getVertex(vertexBuffer, NULL, 0) + vertexDispatch->m_getPositionOffset(vertexBuffer, NULL);
	LegoU32 stride = vertexDispatch->m_getStride(vertexBuffer, NULL);
	vertexDispatch->m_setCursor(vertexBuffer, NULL, 0);
	LegoS32 outputVertexCount = 0;
	LegoU8* group = (LegoU8*) mesh->m_groupData;
	LegoU8* groupEnd = group + mesh->m_groupCount * 0x24;
	while (group < groupEnd) {
		LegoS16 depthIndexA = *(LegoS16*) (group + 0x1c);
		LegoS16 depthIndexB = *(LegoS16*) (group + 0x1e);
		LegoBool32 pointAInFront = IsStencilPointInFront(depthIndexA);
		LegoBool32 emitVolumeSide = pointAInFront;
		LegoS16 edgeIndex = *(LegoS16*) (group + 0x18);
		if (*(LegoU32*) (group + 0x20) == 2 && IsStencilPointInFront(depthIndexB)) {
			emitVolumeSide = !pointAInFront;
			if (emitVolumeSide) {
				edgeIndex = *(LegoS16*) (group + 0x1a);
			}
		}

		if (emitVolumeSide) {
			const LegoFloat* edgeStart = (const LegoFloat*) (group + (LegoS32) edgeIndex * 0x0c);
			const LegoFloat* edgeEnd = (const LegoFloat*) (group - (LegoS32) edgeIndex * 0x0c + 0x0c);
			CopyStencilPoint(destination, edgeStart);
			destination += stride;
			CopyStencilPoint(destination, edgeEnd);
			destination += stride;
			Vector3 extrudedStart = *(const Vector3*) edgeStart;
			extrudedStart.m_x += extrusion.m_x;
			extrudedStart.m_y += extrusion.m_y;
			extrudedStart.m_z += extrusion.m_z;
			CopyStencilPoint(destination, &extrudedStart.m_x);
			destination += stride;
			Vector3 extrudedEnd = *(const Vector3*) edgeEnd;
			extrudedEnd.m_x += extrusion.m_x;
			extrudedEnd.m_y += extrusion.m_y;
			extrudedEnd.m_z += extrusion.m_z;
			CopyStencilPoint(destination, &extrudedEnd.m_x);
			destination += stride;
			outputVertexCount += 4;
		}
		group += 0x24;
	}

	vertexDispatch->m_unlock(vertexBuffer, NULL, 1);
	if (outputVertexCount > 0) {
		rendererDispatch->m_setWorldQuaternion(
			renderer,
			NULL,
			(const Quaternion*) (resource + 0x8c),
			(const Vector3*) (resource + 0x9c)
		);
		rendererDispatch->m_drawStencilVolumes(renderer, NULL, outputVertexCount);
	}
}

static LegoU32 ScaleShadowChannel(LegoU32 p_channel, LegoU32 p_factor)
{
	return ((p_channel & 0xff) * p_factor) >> 8;
}

static LegoU32 ScaleShadowColor(LegoU32 p_color, LegoU32 p_factor)
{
	return ScaleShadowChannel(p_color, p_factor) | (ScaleShadowChannel(p_color >> 8, p_factor) << 8) |
		   (ScaleShadowChannel(p_color >> 16, p_factor) << 16) | (ScaleShadowChannel(p_color >> 24, p_factor) << 24);
}

static LegoS32 RoundShadowFloatWithCurrentFpuMode(LegoFloat p_value)
{
	LegoS32 result;
	__asm {
		fld p_value
		fistp result
	}
	return result;
}

// FUNCTION length: 0x5e1 bytes (0x004c6fe0-0x004c75c0).
// STUB: LEGORACERS2 0x004c6fe0
void __fastcall RenderDeferredResource(DisplayRenderer* p_renderer, void*, void* p_resource)
{
	LegoU8* resource = (LegoU8*) p_resource;
	LegoU8* resourceOwner = *(LegoU8**) (resource + 8);
	RendererStencilMeshView* mesh = *(RendererStencilMeshView**) (resourceOwner + 0x28);
	if (mesh->m_enabled == 0 || p_renderer->m_unk0x4c0 < 1) {
		return;
	}

	RendererDrawPacketView* packet = (RendererDrawPacketView*) p_renderer->m_unk0x4b4;
	LegoS32 selectedLight = *(LegoS32*) ((LegoU8*) mesh + 0x28);
	if (selectedLight == -1) {
		selectedLight = *(LegoS32*) ((LegoU8*) p_renderer + 0x4bc);
	}
	*(LegoU16*) ((LegoU8*) packet + 0x20) = (LegoU16) selectedLight;
	RendererVertexBuffer* vertexBuffer = packet->m_vertices;
	RendererIndexBuffer* indexBuffer = packet->m_indices;

	Matrix3 objectRotation;
	objectRotation.FUN_004a4d30((const LegoFloat*) (resource + 0x8c));
	const LegoFloat* rotation = &objectRotation.m_values[0][0];
	Vector3 shadowAxis;
	shadowAxis.m_x = rotation[1];
	shadowAxis.m_y = 0.0f;
	shadowAxis.m_z = rotation[7];
	LegoFloat axisLength = shadowAxis.Normalize();
	LegoFloat horizontal = shadowAxis.m_x - 0.5f;
	LegoFloat vertical = 0.5f - shadowAxis.m_z;
	LegoFloat shadowAmount = (rotation[3] - 0.5f) * 2.0f * (1.0f - *(LegoFloat*) (resource + 0xc0));
	LegoFloat alphaScaled = shadowAmount * 255.0f;
	LegoS32 alpha = RoundShadowFloatWithCurrentFpuMode(alphaScaled);
	if (alpha < 0) {
		alpha = 0;
	}
	else if (alpha > 255) {
		alpha = 255;
	}
	LegoU32 baseColor = ScaleShadowColor(0xa0a0a0a0, (LegoU32) alpha);

	Vector3 resourceDirection = *(Vector3*) (resource + 0x9c);
	Vector3 terrainOrigin;
	terrainOrigin.m_x = 0.0f;
	terrainOrigin.m_y = 0.0f;
	terrainOrigin.m_z = 0.0f;
	Vector3 corners[4];
	corners[0].m_x = mesh->m_centerX - mesh->m_halfWidth;
	corners[0].m_y = 0.0f;
	corners[0].m_z = mesh->m_halfHeight + mesh->m_centerZ;
	corners[1].m_x = mesh->m_centerX + mesh->m_halfWidth;
	corners[1].m_y = 0.0f;
	corners[1].m_z = mesh->m_centerZ - mesh->m_halfHeight;
	corners[2].m_x = mesh->m_centerX + mesh->m_halfWidth;
	corners[2].m_y = 0.0f;
	corners[2].m_z = mesh->m_halfHeight + mesh->m_centerZ;
	corners[3].m_x = mesh->m_centerX - mesh->m_halfWidth;
	corners[3].m_y = 0.0f;
	corners[3].m_z = mesh->m_centerZ - mesh->m_halfHeight;
	LegoFloat inverseWidth = 1.0f / (mesh->m_halfWidth + mesh->m_halfWidth);
	LegoFloat inverseHeight = 1.0f / (mesh->m_halfHeight + mesh->m_halfHeight);
	for (LegoS32 i = 0; i < 4; ++i) {
		Vector3* point = &corners[i];
		LegoFloat x = horizontal * point->m_z + vertical * point->m_x;
		LegoFloat z = shadowAxis.m_x * point->m_x + shadowAxis.m_z * point->m_z;
		point->m_x = resourceDirection.m_x + x;
		point->m_y = resourceDirection.m_y;
		point->m_z = resourceDirection.m_z + z;
	}

	LegoFloat lowerY;
	LegoFloat upperY;
	void* heightRange = *(void**) (resourceOwner + 0x14);
	if (heightRange != NULL) {
		lowerY = *(LegoFloat*) ((LegoU8*) heightRange + 4) + resourceDirection.m_y;
		upperY = *(LegoFloat*) ((LegoU8*) heightRange + 0x10) + resourceDirection.m_y;
	}
	else {
		LegoFloat extent = *(LegoFloat*) (resourceOwner + 8);
		lowerY = resourceDirection.m_y - extent;
		upperY = resourceDirection.m_y + extent;
	}

	((RendererVertexBufferDispatch*) vertexBuffer->m_vtable)->m_setBufferPolicy(vertexBuffer, NULL, 1);
	((RendererIndexBufferDispatch*) indexBuffer->m_vtable)->m_setBufferPolicy(indexBuffer, NULL, 1);

	for (LegoS32 itemIndex = 0; itemIndex < (LegoS32) p_renderer->m_resourceWorkList->m_count; ++itemIndex) {
		LegoU8* item =
			(LegoU8*) p_renderer->m_resourceWorkList->m_elements + p_renderer->m_resourceWorkList->m_stride * itemIndex;
		void* itemGeometry = *(void**) (item + 0x54);
		typedef Vector3*(__fastcall * GetItemVector)(void*, void*);
		typedef LegoFloat*(__fastcall * GetItemMatrix)(void*, void*);
		void** itemVtable = *(void***) itemGeometry;
		Vector3 itemPosition = *((GetItemVector) itemVtable[0x14 / 4])(itemGeometry, NULL);
		LegoFloat localTransformValues[9];
		memcpy(
			localTransformValues,
			((GetItemMatrix) itemVtable[0x18 / 4])(itemGeometry, NULL),
			sizeof(localTransformValues)
		);
		Matrix3 localTransform;
		memcpy(localTransform.m_values, localTransformValues, sizeof(localTransformValues));

		packet->m_primitiveCount = 0;
		if (!((RendererVertexBufferDispatch*) vertexBuffer->m_vtable)->m_lock(vertexBuffer, NULL, 0, 0, 0, 0)) {
			continue;
		}
		if (!((RendererIndexBufferDispatch*) indexBuffer->m_vtable)->m_lock(indexBuffer, NULL, 0, 0, 1)) {
			((RendererVertexBufferDispatch*) vertexBuffer->m_vtable)->m_unlock(vertexBuffer, NULL, 0);
			continue;
		}
		RendererMainDispatch* dispatch = (RendererMainDispatch*) p_renderer->m_vtable;
		dispatch->m_buildTerrainGeometry(
			(Terrain*) item,
			&terrainOrigin,
			corners,
			4,
			(RendererTerrainGeometryPacket*) packet,
			0
		);
		Vector3 worldOffset = terrainOrigin;
		worldOffset.FUN_004a4e00(&localTransform);
		worldOffset.m_x += itemPosition.m_x;
		worldOffset.m_y += itemPosition.m_y;
		worldOffset.m_z += itemPosition.m_z;
		worldOffset.m_x -= resourceDirection.m_x;
		worldOffset.m_y -= resourceDirection.m_y;
		worldOffset.m_z -= resourceDirection.m_z;
		if (packet->m_primitiveCount != 0 && upperY > worldOffset.m_y) {
			LegoFloat distance = lowerY - worldOffset.m_y;
			if (distance < 0.0f) {
				distance = 0.0f;
			}
			else if (distance > 1000.0f) {
				distance = 1000.0f;
			}
			LegoFloat fade = (1.0f - distance * 0.0001f) * shadowAmount * 255.0f;
			LegoS32 fadeInt = RoundShadowFloatWithCurrentFpuMode(fade);
			if (fadeInt < 0) {
				fadeInt = 0;
			}
			else if (fadeInt > 255) {
				fadeInt = 255;
			}
			LegoU32 color = ScaleShadowColor(baseColor, (LegoU32) fadeInt);
			RendererVertexBufferDispatch* vertexDispatch = (RendererVertexBufferDispatch*) vertexBuffer->m_vtable;
			vertexDispatch->m_setCursor(vertexBuffer, NULL, 0);
			for (LegoS32 vertexIndex = 0; vertexIndex < (LegoS32) packet->m_vertexCount; ++vertexIndex) {
				Vector3* position = vertexDispatch->m_getCurrentPosition(vertexBuffer, NULL);
				Vector3 transformed = *position;
				transformed.FUN_004a4e00(&localTransform);
				transformed.m_x += worldOffset.m_x;
				transformed.m_y += worldOffset.m_y;
				transformed.m_z += worldOffset.m_z;
				LegoFloat projectedX =
					(transformed.m_x * vertical + transformed.m_z * horizontal) * inverseWidth + 0.5f;
				LegoFloat projectedY =
					(transformed.m_x * shadowAxis.m_x + transformed.m_z * shadowAxis.m_z) * inverseHeight + 0.5f;
				RendererTextureCoordinate uv;
				uv.m_u = projectedX;
				uv.m_v = projectedY;
				vertexDispatch->m_setCurrentTextureCoordinate(vertexBuffer, NULL, 0, &uv);
				vertexDispatch->m_setCurrentColor(vertexBuffer, NULL, &color);
				vertexDispatch->m_advanceCursor(vertexBuffer, NULL);
			}
		}
		((RendererIndexBufferDispatch*) indexBuffer->m_vtable)->m_unlock(indexBuffer, NULL, 1, 0);
		((RendererVertexBufferDispatch*) vertexBuffer->m_vtable)->m_unlock(vertexBuffer, NULL, 1);
		if (packet->m_primitiveCount != 0) {
			RendererMainDispatch* dispatch = (RendererMainDispatch*) p_renderer->m_vtable;
			dispatch->m_setWorldMatrix(p_renderer, NULL, &localTransform, &itemPosition);
			dispatch->m_drawRendererPacket(p_renderer, NULL, packet);
		}
	}
	((RendererIndexBufferDispatch*) indexBuffer->m_vtable)->m_setBufferPolicy(indexBuffer, NULL, 0);
	((RendererVertexBufferDispatch*) vertexBuffer->m_vtable)->m_setBufferPolicy(vertexBuffer, NULL, 0);
}
