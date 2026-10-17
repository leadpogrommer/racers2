#include "memoryheap.h"
#include "renderer.h"
#include "rendererbufferdispatch.h"
#include "rendererstageplan.h"
#include "vector3.h"

#include <math.h>

namespace
{
const LegoU32 k_textureOperationDotProduct3Bit = 0x00800000;
const LegoFloat k_bumpVectorLengthThreshold = 0.0001f;

void NormalizeBumpVector(Vector3* p_vector)
{
	const LegoFloat magnitude =
		(LegoFloat) sqrt(p_vector->m_x * p_vector->m_x + p_vector->m_y * p_vector->m_y + p_vector->m_z * p_vector->m_z);
	if (magnitude > k_bumpVectorLengthThreshold) {
		const LegoFloat reciprocal = 1.0f / magnitude;
		p_vector->m_x *= reciprocal;
		p_vector->m_y *= reciprocal;
		p_vector->m_z *= reciprocal;
	}
}
} // namespace

// STUB: LEGORACERS2 0x004de460
void DisplayRenderer::BuildBumpMapVectors(RendererStagePlanRequest* p_request)
{
	if ((GetStaticDisplayRenderer()->m_selectedAdapter->m_caps.TextureOpCaps & k_textureOperationDotProduct3Bit) != 0) {
		return;
	}

	RendererVertexBuffer* vertexBuffer = (RendererVertexBuffer*) p_request->m_unk0x0c;
	RendererIndexBuffer* indexBuffer = (RendererIndexBuffer*) p_request->m_unk0x10;
	const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertexBuffer->m_vtable;
	(void) dispatch->m_getStride(vertexBuffer, NULL);

	LegoU32 textureStage = 0x0f;
	for (LegoS16 i = 0; i < (LegoS16) p_request->m_defaults.m_unk0x04; i++) {
		if (p_request->m_defaults.m_slots[i].m_value == 0) {
			textureStage = p_request->m_defaults.m_slots[i].m_kind & 0x0f;
			break;
		}
	}

	const LegoU32 faceCount = p_request->m_unk0x00;
	const LegoU32 vertexCount = p_request->m_unk0x02;
	if (indexBuffer->m_unk0x0c != 0 || p_request->m_heap != NULL) {
		return;
	}

	Vector3* vertexVectors = (Vector3*) GetMemoryHeap()->Allocate(vertexCount * 0x18, "Bump map vectors");
	p_request->m_heap = vertexVectors;
	Vector3* faceVectors = (Vector3*) GetMemoryHeap()->AllocateFromEnd(faceCount * 0x18, "FaceDirections");
	Vector3* faceTangents = faceVectors;
	Vector3* faceBitangents = faceVectors + faceCount;

	const LegoU16* indices = (const LegoU16*) indexBuffer->m_data;
	if (dispatch->m_lock(vertexBuffer, NULL, 0, vertexCount, 0, 0) == TRUE) {
		for (LegoU32 face = 0; face < faceCount; face++) {
			const LegoU16 index0 = indices[face * 3 + 0];
			const LegoU16 index1 = indices[face * 3 + 1];
			const LegoU16 index2 = indices[face * 3 + 2];
			const Vector3* position0 = dispatch->m_getPosition(vertexBuffer, NULL, index0);
			const RendererTextureCoordinate* uv0 =
				dispatch->m_getTextureCoordinate(vertexBuffer, NULL, index0, textureStage);
			const Vector3* position1 = dispatch->m_getPosition(vertexBuffer, NULL, index1);
			const RendererTextureCoordinate* uv1 =
				dispatch->m_getTextureCoordinate(vertexBuffer, NULL, index1, textureStage);
			const Vector3* position2 = dispatch->m_getPosition(vertexBuffer, NULL, index2);
			const RendererTextureCoordinate* uv2 =
				dispatch->m_getTextureCoordinate(vertexBuffer, NULL, index2, textureStage);

			const LegoFloat edge01X = position0->m_x - position1->m_x;
			const LegoFloat edge01Y = position0->m_y - position1->m_y;
			const LegoFloat edge01Z = position0->m_z - position1->m_z;
			const LegoFloat edge12X = position1->m_x - position2->m_x;
			const LegoFloat edge12Y = position1->m_y - position2->m_y;
			const LegoFloat edge12Z = position1->m_z - position2->m_z;
			const LegoFloat uv01U = uv0->m_u - uv1->m_u;
			const LegoFloat uv01V = uv0->m_v - uv1->m_v;
			const LegoFloat uv12U = uv1->m_u - uv2->m_u;
			const LegoFloat uv12V = uv1->m_v - uv2->m_v;

			Vector3 tangent;
			tangent.m_x = uv12V * edge01X - edge12X * uv01V;
			tangent.m_y = uv12V * edge01Y - edge12Y * uv01V;
			tangent.m_z = uv12V * edge01Z - edge12Z * uv01V;
			NormalizeBumpVector(&tangent);

			Vector3 bitangent;
			bitangent.m_x = edge12X * uv01U - uv12U * edge01X;
			bitangent.m_y = edge12Y * uv01U - uv12U * edge01Y;
			bitangent.m_z = edge12Z * uv01U - uv12U * edge01Z;
			NormalizeBumpVector(&bitangent);

			faceTangents[face] = tangent;
			faceBitangents[face] = bitangent;
		}
		dispatch->m_unlock(vertexBuffer, NULL, 0);
	}

	for (LegoU32 vertex = 0; vertex < vertexCount; vertex++) {
		Vector3 tangentSum = {0.0f, 0.0f, 0.0f};
		Vector3 bitangentSum = {0.0f, 0.0f, 0.0f};
		indices = (const LegoU16*) indexBuffer->m_data;
		for (LegoU32 face = 0; face < faceCount; face++) {
			for (LegoU32 corner = 0; corner < 3; corner++) {
				if (indices[face * 3 + corner] == vertex) {
					tangentSum.m_x += faceTangents[face].m_x;
					tangentSum.m_y += faceTangents[face].m_y;
					tangentSum.m_z += faceTangents[face].m_z;
					bitangentSum.m_x += faceBitangents[face].m_x;
					bitangentSum.m_y += faceBitangents[face].m_y;
					bitangentSum.m_z += faceBitangents[face].m_z;
				}
			}
		}
		NormalizeBumpVector(&tangentSum);
		NormalizeBumpVector(&bitangentSum);
		vertexVectors[vertex * 2] = tangentSum;
		vertexVectors[vertex * 2 + 1] = bitangentSum;
	}

	FreeHeapMemory(faceVectors);
}
