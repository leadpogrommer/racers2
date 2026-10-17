#ifndef LR2_RENDERERBUFFEROBJECTS_H
#define LR2_RENDERERBUFFEROBJECTS_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

struct RendererVertexStorage {
	LegoBool32 Initialize(LegoU32 p_attributes, LegoS32 p_count, LegoS32 p_textureCount, LegoU32 p_mode);
	void Reserve(LegoS32 p_vertices, LegoS32* p_baseVertex, LegoU8** p_data);
	BYTE* Lock(LegoS32 p_start, LegoS32 p_count, LegoU32 p_mode, LegoU32 p_discard);
	void Unlock();
	void ReleaseReference();
	void ReleaseDeviceBuffer();
	IDirect3DVertexBuffer8* m_deviceBuffer; // 0x00
	LegoU32 m_flags;                        // 0x04
	LegoU32 m_format;                       // 0x08
	LegoS32 m_nextVertex;                   // 0x0c
	LegoS32 m_capacity;                     // 0x10
	LegoS32 m_available;                    // 0x14
	LegoS32 m_stride;                       // 0x18
	LegoU8* m_base;                         // 0x1c
	LegoS32 m_references;                   // 0x20
};
DECOMP_SIZE_ASSERT(RendererVertexStorage, 0x24);

class Vector3;
struct RendererVertexDataView;
struct RendererTextureCoordinate {
	LegoFloat m_u;
	LegoFloat m_v;
};

class RendererVertexBuffer {
public:
	void CopyFromVertexData(RendererVertexDataView* p_source, LegoS32 p_start);
	void CopyVerticesFrom(RendererVertexBuffer* p_source, LegoS32 p_start);
	void CopyToVertexData(RendererVertexDataView* p_target);
	void SetMode(LegoS32 p_mode);
	IDirect3DVertexBuffer8* GetDeviceBuffer();
	LegoU32 GetAttributes();
	LegoU32 GetCount();
	LegoS32 GetTextureCount();
	LegoU8* GetVertex(LegoS32 p_index);
	LegoU32 GetPositionOffset();
	LegoU32 GetNormalOffset();
	LegoU32 GetColorOffset();
	LegoU32 GetTextureOffset();
	void SetCursor(LegoS32 p_index);
	void AdvanceCursor();
	Vector3* GetCurrentPosition();
	Vector3* GetPosition(LegoS32 p_index);
	Vector3* GetCurrentNormal();
	Vector3* GetNormal(LegoS32 p_index);
	LegoU32* GetCurrentColor();
	LegoU32* GetColor(LegoS32 p_index);
	RendererTextureCoordinate* GetCurrentTextureCoordinate(LegoS32 p_stage);
	RendererTextureCoordinate* GetTextureCoordinate(LegoS32 p_index, LegoS32 p_stage);
	void SetCurrentPosition(const Vector3* p_value);
	void SetPosition(LegoS32 p_index, const Vector3* p_value);
	void SetCurrentNormal(const Vector3* p_value);
	void SetNormal(LegoS32 p_index, const Vector3* p_value);
	void SetCurrentColor(const LegoU32* p_value);
	void SetColor(LegoS32 p_index, const LegoU32* p_value);
	void SetCurrentTextureCoordinate(LegoS32 p_stage, const RendererTextureCoordinate* p_value);
	void SetTextureCoordinate(LegoS32 p_index, LegoS32 p_stage, const RendererTextureCoordinate* p_value);
	LegoU32 GetStride();
	void Release();
	void ConfigureLayout(LegoU32 p_attributes, LegoS32 p_count, LegoS32 p_textureCount, LegoU32 p_mode);
	void AttachStorage(RendererVertexStorage* p_storage, LegoS32 p_count);
	LegoBool32 ConfigureFromStorage(
		RendererVertexStorage* p_storage,
		LegoU16 p_attributes,
		LegoS32 p_count,
		LegoS32 p_textureCount,
		LegoU32 p_mode
	);
	LegoBool32 Lock(LegoS32 p_start, LegoS32 p_count, LegoU32 p_mode, LegoU32 p_discard);
	void Unlock(LegoU32 p_commit);
	const void* m_vtable; // 0x00
	LegoU32 m_unk0x04;
	LegoU32 m_positionOffset; // 0x08
	LegoU32 m_normalOffset;   // 0x0c
	LegoU32 m_colorOffset;    // 0x10
	LegoU32 m_textureOffset;  // 0x14
	LegoU32 m_stride;         // 0x18
	LegoS32 m_textureCount;   // 0x1c
	LegoU16 m_attributes;     // 0x20
	LegoU16 m_count;          // 0x22
	LegoU16 m_unk0x24;
	undefined m_unk0x26[0x28 - 0x26];
	LegoU8* m_data;                   // 0x28
	LegoU8* m_cursor;                 // 0x2c
	RendererVertexStorage* m_storage; // 0x30
	LegoU8* m_savedData;              // 0x34
	LegoU32 m_savedStride;            // 0x38
	LegoU32 m_generation;             // 0x3c
	LegoS32 m_lockStart;              // 0x40
	LegoS32 m_lockCount;              // 0x44
	LegoS32 m_baseVertex;             // 0x48
	LegoU32 m_flags;                  // 0x4c
};
DECOMP_SIZE_ASSERT(RendererVertexBuffer, 0x50);

// CPU storage shared by renderer index-buffer records.
struct RendererIndexStorage {
	LegoBool32 Initialize(LegoU32 p_bytes);
	void Reserve(LegoU32 p_bytes, LegoU8** p_result);
	void ReleaseReference();
	LegoU32 m_capacity;
	LegoU32 m_available;
	LegoS32 m_references;
	LegoU8* m_base;
	LegoU8* m_cursor;
};
DECOMP_SIZE_ASSERT(RendererIndexStorage, 0x14);

class RendererIndexBuffer {
public:
	void Release();
	LegoBool32 Lock(LegoS32 p_start, LegoS32 p_count, LegoU32 p_mode);
	void Unlock(LegoU32 p_commit, LegoU32 p_mode);
	void SetFlag4(LegoU32 p_enabled);
	void ReleaseDeviceBuffer();
	IDirect3DIndexBuffer8* GetDeviceBuffer();
	void Upload();

	const void* m_vtable; // 0x00, table not emitted until all callbacks are recovered
	LegoS32 m_count;      // 0x04
	LegoU32 m_unk0x08;
	LegoU32 m_unk0x0c;
	LegoU8* m_data;                        // 0x10, base adjusted while device storage is locked
	LegoU8* m_writtenEnd;                  // 0x14
	IDirect3DIndexBuffer8* m_deviceBuffer; // 0x18
	LegoU8* m_savedData;                   // 0x1c
	RendererIndexStorage* m_storage;       // 0x20
	LegoS32 m_lockStart;                   // 0x24
	LegoS32 m_lockCount;                   // 0x28
	LegoU32 m_flags;                       // 0x2c
};
DECOMP_SIZE_ASSERT(RendererIndexBuffer, 0x30);
DECOMP_STATIC_ASSERT(offsetof(RendererIndexBuffer, m_deviceBuffer) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(RendererIndexBuffer, m_flags) == 0x2c);

LegoU32 WINAPI GetVertexFormat(LegoU8 p_attributes, LegoU32 p_textureCount);
LegoS32 WINAPI GetVertexFormatStride(LegoU32 p_format);

#endif
