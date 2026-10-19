#include "memoryheap.h"
#include "modelresource.h"
#include "modelshadow.h"
#include "renderer.h"
#include "rendererbufferdispatch.h"
#include "rendererconstructiondispatch.h"
#include "renderershadowshared.h"
#include "vector3.h"

#include <math.h>
#include <string.h>

namespace
{
struct ShadowTemporaryFace {
	LegoU32 m_used;
	LegoS16 m_vertices[3];
	undefined2 m_unk0x0a;
	Vector3 m_normal;
};
DECOMP_SIZE_ASSERT(ShadowTemporaryFace, 0x18);
DECOMP_STATIC_ASSERT(offsetof(ShadowTemporaryFace, m_normal) == 0x0c);

struct ShadowTemporaryEdge {
	LegoU16 m_vertices[2];
	LegoS16 m_faces[8];
	undefined m_unk0x14[0x24 - 0x14];
	LegoU16 m_directions[8];
	undefined4 m_unk0x34;
	LegoS32 m_count;
};
DECOMP_SIZE_ASSERT(ShadowTemporaryEdge, 0x3c);
DECOMP_STATIC_ASSERT(offsetof(ShadowTemporaryEdge, m_directions) == 0x24);
DECOMP_STATIC_ASSERT(offsetof(ShadowTemporaryEdge, m_count) == 0x38);

struct ShadowGeneratedEdge {
	Vector3 m_vertices[2];
	LegoU16 m_directions[2];
	LegoS16 m_faces[2];
	LegoU32 m_count;
};
DECOMP_SIZE_ASSERT(ShadowGeneratedEdge, 0x24);
DECOMP_STATIC_ASSERT(offsetof(ShadowGeneratedEdge, m_faces) == 0x1c);

ModelTextureGroup* FindShadowGeometryGroup(ModelResource* p_model, LegoU32 p_type)
{
	for (LegoS32 index = 0; index < p_model->m_groupCount; ++index) {
		if (p_model->m_groups[index].m_type == p_type) {
			return &p_model->m_groups[index];
		}
	}
	return NULL;
}

LegoBool32 ShadowVectorsEqual(const Vector3* p_first, const Vector3* p_second)
{
	// Retail rounds each subtraction to float before clearing its sign bit.
	// Its x87 status-word test also accepts unordered comparisons.
	LegoFloat x = p_first->m_x - p_second->m_x;
	LegoFloat y = p_first->m_y - p_second->m_y;
	LegoFloat z = p_first->m_z - p_second->m_z;
	return !(fabs(x) > 0.0001f) && !(fabs(y) > 0.0001f) && !(fabs(z) > 0.0001f);
}

void ReadShadowGeometry(
	ModelTextureGroup* p_group,
	ShadowTemporaryFace* p_faces,
	Vector3* p_vertices,
	LegoS32* p_faceCount,
	LegoS32* p_vertexCount
)
{
	*p_faceCount = 0;
	*p_vertexCount = 0;
	LegoS32 indexBase = 0;
	if (p_group == NULL) {
		return;
	}
	for (LegoS32 primitiveIndex = 0; primitiveIndex < p_group->m_primitiveCount; ++primitiveIndex) {
		ModelPrimitiveTextureRecord* primitive = &p_group->m_primitives[primitiveIndex];
		RendererVertexBuffer* vertices = primitive->m_vertexBuffer;
		RendererIndexBuffer* indices = primitive->m_indexBuffer;
		const RendererVertexBufferDispatch* vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		LegoS32 count = vertexDispatch->m_getCount(vertices, NULL);
		if (indices->m_unk0x0c != 0) {
			continue;
		}
		if (vertexDispatch->m_lock(vertices, NULL, 0, count, 0, 0) == 1) {
			vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			vertexDispatch->m_setCursor(vertices, NULL, 0);
			for (LegoS32 vertex = 0; vertex < count; ++vertex) {
				vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				p_vertices[*p_vertexCount + vertex] = *vertexDispatch->m_getCurrentPosition(vertices, NULL);
				vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				vertexDispatch->m_advanceCursor(vertices, NULL);
			}
			vertexDispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			vertexDispatch->m_unlock(vertices, NULL, 0);
			if (count > 0) {
				*p_vertexCount += count;
			}
		}
		const RendererIndexBufferDispatch* indexDispatch = (const RendererIndexBufferDispatch*) indices->m_vtable;
		if (indexDispatch->m_lock(indices, NULL, 0, (indices->m_writtenEnd - indices->m_data) / 2, 0) == 1) {
			for (LegoS32 faceIndex = 0; faceIndex < primitive->m_primitiveCount; ++faceIndex) {
				ShadowTemporaryFace* face = &p_faces[(*p_faceCount)++];
				const LegoU16* triangle = (const LegoU16*) indices->m_data + faceIndex * 3;
				for (LegoS32 corner = 0; corner < 3; ++corner) {
					face->m_vertices[corner] = (LegoS16) (triangle[corner] + indexBase);
				}
				const Vector3* a = &p_vertices[face->m_vertices[0]];
				const Vector3* b = &p_vertices[face->m_vertices[1]];
				const Vector3* c = &p_vertices[face->m_vertices[2]];
				Vector3 first;
				Vector3 second;
				first.m_x = a->m_x - b->m_x;
				first.m_y = a->m_y - b->m_y;
				first.m_z = a->m_z - b->m_z;
				second.m_x = b->m_x - c->m_x;
				second.m_y = b->m_y - c->m_y;
				second.m_z = b->m_z - c->m_z;
				face->m_normal.m_z = second.m_y * first.m_x - first.m_y * second.m_x;
				face->m_normal.m_x = second.m_z * first.m_y - second.m_y * first.m_z;
				face->m_normal.m_y = first.m_z * second.m_x - second.m_z * first.m_x;
				face->m_normal.Normalize();
				face->m_used = 1;
			}
			indexDispatch = (const RendererIndexBufferDispatch*) indices->m_vtable;
			indexDispatch->m_unlock(indices, NULL, 0, 1);
		}
		indexBase += count;
	}
}

void WeldShadowVertices(ShadowTemporaryFace* p_faces, LegoS32 p_faceCount, Vector3* p_vertices, LegoS32 p_vertexCount)
{
	for (LegoS32 first = 0; first < p_vertexCount; ++first) {
		for (LegoS32 second = first + 1; second < p_vertexCount; ++second) {
			if (ShadowVectorsEqual(&p_vertices[first], &p_vertices[second])) {
				for (LegoS32 face = 0; face < p_faceCount; ++face) {
					for (LegoS32 corner = 0; corner < 3; ++corner) {
						if (p_faces[face].m_vertices[corner] == second) {
							p_faces[face].m_vertices[corner] = (LegoS16) first;
						}
					}
				}
			}
		}
	}
}

void RemoveDuplicateShadowFaces(ShadowTemporaryFace* p_faces, LegoS32 p_faceCount)
{
	for (LegoS32 first = 0; first < p_faceCount; ++first) {
		if (p_faces[first].m_used == 0) {
			continue;
		}
		for (LegoS32 second = first + 1; second < p_faceCount; ++second) {
			if (p_faces[second].m_used == 0) {
				continue;
			}
			LegoS32 corner;
			for (corner = 0; corner < 3; ++corner) {
				LegoS32 other;
				for (other = 0; other < 3; ++other) {
					if (p_faces[first].m_vertices[corner] == p_faces[second].m_vertices[other]) {
						break;
					}
				}
				if (other == 3) {
					break;
				}
			}
			const Vector3* a = &p_faces[first].m_normal;
			const Vector3* b = &p_faces[second].m_normal;
			if (corner == 3 && a->m_x * b->m_x + a->m_z * b->m_z + b->m_y * a->m_y > 0.0f) {
				p_faces[second].m_used = 0;
			}
		}
	}
}

LegoS32 BuildShadowEdges(ShadowTemporaryFace* p_faces, LegoS32 p_faceCount, ShadowTemporaryEdge* p_edges)
{
	LegoS32 count = 0;
	for (LegoS32 face = 0; face < p_faceCount; ++face) {
		if (p_faces[face].m_used == 0) {
			continue;
		}
		for (LegoS32 corner = 0; corner < 3; ++corner) {
			LegoU16 a = (LegoU16) p_faces[face].m_vertices[corner];
			LegoU16 b = (LegoU16) p_faces[face].m_vertices[(corner + 1) % 3];
			LegoS32 index;
			for (index = 0; index < count; ++index) {
				ShadowTemporaryEdge* edge = &p_edges[index];
				if (((edge->m_vertices[0] == a && edge->m_vertices[1] == b) ||
					 (edge->m_vertices[0] == b && edge->m_vertices[1] == a)) &&
					edge->m_count < 8) {
					edge->m_directions[edge->m_count] = edge->m_vertices[0] != a;
					edge->m_faces[edge->m_count++] = (LegoS16) face;
					break;
				}
			}
			if (index == count) {
				ShadowTemporaryEdge* edge = &p_edges[count++];
				edge->m_vertices[0] = a;
				edge->m_vertices[1] = b;
				edge->m_faces[0] = (LegoS16) face;
				edge->m_directions[0] = 0;
				edge->m_count = 1;
			}
		}
	}
	return count;
}

void RemoveCoplanarShadowEdges(ShadowTemporaryFace* p_faces, ShadowTemporaryEdge* p_edges, LegoS32* p_edgeCount)
{
	for (LegoS32 edge = 0; edge < *p_edgeCount; ++edge) {
		ShadowTemporaryEdge* record = &p_edges[edge];
		LegoS32 face = 1;
		for (; face < record->m_count; ++face) {
			if (!ShadowVectorsEqual(&p_faces[record->m_faces[0]].m_normal, &p_faces[record->m_faces[face]].m_normal)) {
				break;
			}
		}
		if (face == record->m_count && record->m_count > 1) {
			*record = p_edges[--*p_edgeCount];
			--edge;
		}
	}
}

LegoS32 CompactShadowNormals(
	ShadowTemporaryFace* p_faces,
	LegoS32 p_faceCount,
	ShadowTemporaryEdge* p_edges,
	LegoS32 p_edgeCount
)
{
	LegoS32 count = 0;
	for (LegoS32 face = 0; face < p_faceCount; ++face) {
		if (p_faces[face].m_used != 1) {
			continue;
		}
		LegoBool32 used = FALSE;
		LegoS32 edge;
		for (edge = 0; edge < p_edgeCount; ++edge) {
			for (LegoS32 index = 0; index < p_edges[edge].m_count; ++index) {
				if (p_edges[edge].m_faces[index] == face) {
					used = TRUE;
				}
			}
		}
		if (!used) {
			continue;
		}
		LegoS32 destination;
		for (destination = 0; destination < count; ++destination) {
			if (face != destination && ShadowVectorsEqual(&p_faces[destination].m_normal, &p_faces[face].m_normal)) {
				break;
			}
		}
		if (destination == count) {
			p_faces[count++] = p_faces[face];
		}
		if (destination != face) {
			for (edge = 0; edge < p_edgeCount; ++edge) {
				for (LegoS32 index = 0; index < p_edges[edge].m_count; ++index) {
					if (p_edges[edge].m_faces[index] == face) {
						p_edges[edge].m_faces[index] = (LegoS16) destination;
					}
				}
			}
		}
	}
	return count;
}
} // namespace

// STUB: LEGORACERS2 0x004a9630
void __fastcall ConfigureModelShadow(
	ModelResource* p_model,
	void*,
	LegoU32 p_enable,
	const LegoChar* p_texture,
	LegoU32 p_generate
)
{
	if (p_enable == 0) {
		ReleaseModelShadowData(p_model, NULL);
	}
	else {
		if (((const RendererMainDispatch*) g_displayRenderer->m_vtable)
				->m_supportsExtendedTextureStages(g_displayRenderer, NULL) == 0) {
			p_generate = 0;
		}
		GenerateModelShadowData(p_model, NULL, p_generate, p_texture);
		if (p_generate == 1) {
			p_model->m_hasSpecialPrimitives = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004c5bb0
void __fastcall GenerateModelShadowData(ModelResource* p_model, void*, LegoU32 p_generate, const LegoChar* p_texture)
{
	EnsureSharedShadowGeometry(g_displayRenderer, NULL);
	if (p_model->m_unk0x28 == NULL) {
		ModelShadowData* storage = (ModelShadowData*) AllocateHeapMemory(sizeof(ModelShadowData));
		if (storage != NULL) {
			memset(storage, 0, sizeof(*storage));
			storage->m_unk0x10 = 1;
			storage->m_scaleX = storage->m_scaleY = 1.0f;
			storage->m_textureId = -1;
		}
		p_model->m_unk0x28 = storage;
	}
	ModelShadowData* shadow = (ModelShadowData*) p_model->m_unk0x28;
	LegoS32 reference = (LegoS32) shadow->m_referenceCount++;
	if (p_texture != NULL) {
		shadow->m_textureId = g_displayRenderer->CreateTextureResource(p_texture, 0, -1, 1, 1);
		if (reference < 32) {
			shadow->m_textureReferenceMask |= 1u << (reference & 31);
		}
	}
	if (p_generate == 0) {
		return;
	}
	if (shadow->m_faceRecords == NULL) {
		ModelTextureGroup* group = FindShadowGeometryGroup(p_model, 2);
		if (group == NULL) {
			group = FindShadowGeometryGroup(p_model, 1);
		}
		LegoS32 maximumFaces = 0;
		if (group != NULL) {
			for (LegoS32 index = 0; index < group->m_primitiveCount; ++index) {
				maximumFaces += group->m_primitives[index].m_primitiveCount;
			}
		}
		if (maximumFaces > 1000) {
			return;
		}
		ShadowTemporaryFace* faces =
			(ShadowTemporaryFace*) GetMemoryHeap()->AllocateFromEnd(24000, "Temp shadow faces");
		ShadowTemporaryEdge* edges =
			(ShadowTemporaryEdge*) GetMemoryHeap()->AllocateFromEnd(180000, "Temp shadow edges");
		Vector3* vertices = (Vector3*) GetMemoryHeap()->AllocateFromEnd(36000, "Temp shadow vectors");
		LegoS32 faceCount;
		LegoS32 vertexCount;
		LegoS32 edgeCount;
		LegoBool32 retried = FALSE;
		for (;;) {
			ReadShadowGeometry(group, faces, vertices, &faceCount, &vertexCount);
			if (!retried) {
				WeldShadowVertices(faces, faceCount, vertices, vertexCount);
			}
			RemoveDuplicateShadowFaces(faces, faceCount);
			edgeCount = BuildShadowEdges(faces, faceCount, edges);
			LegoBool32 nonManifold = FALSE;
			if (!retried) {
				for (LegoS32 edge = 0; edge < edgeCount; ++edge) {
					if (edges[edge].m_count > 2) {
						nonManifold = TRUE;
					}
				}
			}
			if (!nonManifold) {
				break;
			}
			retried = TRUE;
		}
		RemoveCoplanarShadowEdges(faces, edges, &edgeCount);
		LegoS32 normalCount = CompactShadowNormals(faces, faceCount, edges, edgeCount);
		LegoS32 outputCount = 0;
		LegoS32 edge;
		for (edge = 0; edge < edgeCount; ++edge) {
			outputCount += edges[edge].m_count < 3 ? 1 : edges[edge].m_count;
		}
		shadow->m_faceRecords = GetMemoryHeap()->Allocate(normalCount * 0x0c, "Shadow face info");
		shadow->m_edgeRecords = GetMemoryHeap()->Allocate(outputCount * 0x24, "Shadow edge info");
		shadow->m_edgeRecordCount = outputCount;
		shadow->m_faceRecordCount = normalCount;
		for (LegoS32 normal = 0; normal < normalCount; ++normal) {
			((Vector3*) shadow->m_faceRecords)[normal] = faces[normal].m_normal;
		}
		ShadowGeneratedEdge* output = (ShadowGeneratedEdge*) shadow->m_edgeRecords;
		for (edge = 0; edge < edgeCount; ++edge) {
			ShadowTemporaryEdge* input = &edges[edge];
			LegoS32 records = input->m_count < 3 ? 1 : input->m_count;
			for (LegoS32 record = 0; record < records; ++record) {
				output->m_vertices[0] = vertices[input->m_vertices[0]];
				output->m_vertices[1] = vertices[input->m_vertices[1]];
				output->m_count = input->m_count < 3 ? input->m_count : 1;
				for (LegoU32 face = 0; face < output->m_count; ++face) {
					LegoS32 source = input->m_count < 3 ? face : record;
					output->m_directions[face] = input->m_directions[source];
					output->m_faces[face] = input->m_faces[source];
				}
				++output;
			}
		}
		GetMemoryHeap()->Free(vertices);
		GetMemoryHeap()->Free(edges);
		GetMemoryHeap()->Free(faces);
	}
	if (reference < 32) {
		shadow->m_geometryReferenceMask |= 1u << (reference & 31);
	}
}
