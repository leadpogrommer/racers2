#ifndef LR2_RENDERERBUFFERDISPATCH_H
#define LR2_RENDERERBUFFERDISPATCH_H

#include "rendererbufferobjects.h"
#include "renderervertexdata.h"

struct RendererVertexBufferDispatch {
	typedef void(__fastcall* Release)(RendererVertexBuffer*, void*);
	typedef LegoBool32(__fastcall* Lock)(RendererVertexBuffer*, void*, LegoS32, LegoS32, LegoU32, LegoU32);
	typedef void(__fastcall* Unlock)(RendererVertexBuffer*, void*, LegoU32);
	typedef void(__fastcall* CopyVertices)(RendererVertexBuffer*, void*, RendererVertexBuffer*, LegoS32);
	typedef void(__fastcall* CopyFromData)(RendererVertexBuffer*, void*, RendererVertexDataView*, LegoS32);
	typedef void(__fastcall* CopyToData)(RendererVertexBuffer*, void*, RendererVertexDataView*);
	typedef void(__fastcall* SetValue)(RendererVertexBuffer*, void*, LegoU32);
	typedef int(__cdecl* Purecall)(void);
	typedef IDirect3DVertexBuffer8*(__fastcall* GetDeviceBuffer)(RendererVertexBuffer*, void*);
	typedef LegoU32(__fastcall* GetValue)(RendererVertexBuffer*, void*);
	typedef LegoS32(__fastcall* GetSignedValue)(RendererVertexBuffer*, void*);
	typedef LegoU8*(__fastcall* GetVertex)(RendererVertexBuffer*, void*, LegoS32);
	typedef void(__fastcall* SetCursor)(RendererVertexBuffer*, void*, LegoS32);
	typedef void(__fastcall* AdvanceCursor)(RendererVertexBuffer*, void*);
	typedef Vector3*(__fastcall* GetVector)(RendererVertexBuffer*, void*, LegoS32);
	typedef Vector3*(__fastcall* GetCurrentVector)(RendererVertexBuffer*, void*);
	typedef LegoU32*(__fastcall* GetColor)(RendererVertexBuffer*, void*, LegoS32);
	typedef LegoU32*(__fastcall* GetCurrentColor)(RendererVertexBuffer*, void*);
	typedef RendererTextureCoordinate*(__fastcall*
										   GetTextureCoordinate)(RendererVertexBuffer*, void*, LegoS32, LegoS32);
	typedef RendererTextureCoordinate*(__fastcall* GetCurrentTextureCoordinate)(RendererVertexBuffer*, void*, LegoS32);
	typedef void(__fastcall* SetVector)(RendererVertexBuffer*, void*, LegoS32, const Vector3*);
	typedef void(__fastcall* SetCurrentVector)(RendererVertexBuffer*, void*, const Vector3*);
	typedef void(__fastcall* SetColorAt)(RendererVertexBuffer*, void*, LegoS32, const LegoU32*);
	typedef void(__fastcall* SetCurrentColor)(RendererVertexBuffer*, void*, const LegoU32*);
	typedef void(__fastcall* SetTextureCoordinate)(
		RendererVertexBuffer*,
		void*,
		LegoS32,
		LegoS32,
		const RendererTextureCoordinate*
	);
	typedef void(__fastcall* SetCurrentTextureCoordinate)(
		RendererVertexBuffer*,
		void*,
		LegoS32,
		const RendererTextureCoordinate*
	);
	typedef void(__fastcall* LegacyConditionalFree)(RendererVertexBuffer*, void*);

	Release m_release;                                         // 0x00
	Lock m_lock;                                               // 0x04
	Unlock m_unlock;                                           // 0x08
	CopyVertices m_copyVerticesFrom;                           // 0x0c
	CopyFromData m_copyFromVertexData;                         // 0x10
	CopyToData m_copyToVertexData;                             // 0x14
	SetValue m_setMode;                                        // 0x18
	SetValue m_setBufferPolicy;                                // 0x1c
	Purecall m_abstract0x20;                                   // 0x20
	GetDeviceBuffer m_getDeviceBuffer;                         // 0x24
	GetValue m_getAttributes;                                  // 0x28
	GetValue m_getStride;                                      // 0x2c
	GetValue m_getCount;                                       // 0x30
	GetSignedValue m_getTextureCount;                          // 0x34
	GetVertex m_getVertex;                                     // 0x38
	GetValue m_getPositionOffset;                              // 0x3c
	GetValue m_getNormalOffset;                                // 0x40
	GetValue m_getColorOffset;                                 // 0x44
	GetValue m_getTextureOffset;                               // 0x48
	SetCursor m_setCursor;                                     // 0x4c
	AdvanceCursor m_advanceCursor;                             // 0x50
	GetVector m_getPosition;                                   // 0x54
	GetCurrentVector m_getCurrentPosition;                     // 0x58
	GetVector m_getNormal;                                     // 0x5c
	GetCurrentVector m_getCurrentNormal;                       // 0x60
	GetColor m_getColor;                                       // 0x64
	GetCurrentColor m_getCurrentColor;                         // 0x68
	GetTextureCoordinate m_getTextureCoordinate;               // 0x6c
	GetCurrentTextureCoordinate m_getCurrentTextureCoordinate; // 0x70
	SetVector m_setPosition;                                   // 0x74
	SetCurrentVector m_setCurrentPosition;                     // 0x78
	SetVector m_setNormal;                                     // 0x7c
	SetCurrentVector m_setCurrentNormal;                       // 0x80
	SetColorAt m_setColor;                                     // 0x84
	SetCurrentColor m_setCurrentColor;                         // 0x88
	SetTextureCoordinate m_setTextureCoordinate;               // 0x8c
	SetCurrentTextureCoordinate m_setCurrentTextureCoordinate; // 0x90
	LegacyConditionalFree m_legacyConditionalFree;             // 0x94
	Purecall m_abstract0x98;                                   // 0x98
	Purecall m_abstract0x9c;                                   // 0x9c
	Purecall m_abstract0xa0;                                   // 0xa0
	Purecall m_abstract0xa4;                                   // 0xa4
	Purecall m_abstract0xa8;                                   // 0xa8
	Purecall m_abstract0xac;                                   // 0xac
};

DECOMP_SIZE_ASSERT(RendererVertexBufferDispatch, 0xb0);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexBufferDispatch, m_setBufferPolicy) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexBufferDispatch, m_getTextureCoordinate) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexBufferDispatch, m_legacyConditionalFree) == 0x94);

struct RendererIndexBufferDispatch {
	typedef void(__fastcall* Release)(RendererIndexBuffer*, void*);
	typedef LegoBool32(__fastcall* Lock)(RendererIndexBuffer*, void*, LegoS32, LegoS32, LegoU32);
	typedef void(__fastcall* Unlock)(RendererIndexBuffer*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* SetValue)(RendererIndexBuffer*, void*, LegoU32);
	typedef int(__cdecl* Purecall)(void);
	typedef IDirect3DIndexBuffer8*(__fastcall* GetDeviceBuffer)(RendererIndexBuffer*, void*);

	Release m_release;                 // 0x00
	Lock m_lock;                       // 0x04
	Unlock m_unlock;                   // 0x08
	SetValue m_setFlag4;               // 0x0c
	SetValue m_setBufferPolicy;        // 0x10
	Purecall m_abstract0x14;           // 0x14
	GetDeviceBuffer m_getDeviceBuffer; // 0x18
};

DECOMP_SIZE_ASSERT(RendererIndexBufferDispatch, 0x1c);
DECOMP_STATIC_ASSERT(offsetof(RendererIndexBufferDispatch, m_setBufferPolicy) == 0x10);

// These are source-side callback tables wired into the recovered buffer
// records. They are plain typed dispatch data, not annotated original vtables.
extern const RendererVertexBufferDispatch g_rendererVertexBufferDispatch;
extern const RendererIndexBufferDispatch g_rendererIndexBufferDispatch;

#endif
