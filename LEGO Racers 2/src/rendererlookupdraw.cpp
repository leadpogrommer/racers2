#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererdrawpacketview.h"
#include "rendererstageplan.h"

#include <stdlib.h>
#include <string.h>

extern void __fastcall ApplyLookupResourceState(void* p_owner, void*, void* p_resource);

struct RendererLookupGlobals {
	LegoU32 m_mode;
	LegoU32 m_unk0x04;
	LegoU8 m_selectionOriginInitialized;
	LegoU8 m_padding[7];
	LegoFloat m_selectionOrigin[3];
};

DECOMP_SIZE_ASSERT(RendererLookupGlobals, 0x1c);

// GLOBAL: LEGORACERS2 0x0058a170
RendererLookupGlobals g_unk0x0058a170;

// GLOBAL: LEGORACERS2 0x00679ef0
void* g_unk0x00679ef0;

struct LookupResourceListView {
	LegoU8 m_opaque0x00[8];
	LegoS32 m_count;
	LegoU32 m_opaque0x0c;
	LegoU8* m_items;
};

struct LookupProfileRecordView {
	LegoU32 m_opaque0x00[2];
	LegoS32 m_count;
	LegoU32 m_opaque0x0c;
	LegoU8* m_items;
};

// STUB: LEGORACERS2 0x004a74b0
LegoBool32 __fastcall IsLookupPointWithinRadius(void* p_context, void*, const LegoFloat* p_point, LegoFloat p_radius)
{
	LegoU8* context = (LegoU8*) p_context;
	LegoU8* end = *(LegoU8**) (context + 0x90);
	for (LegoU8* entry = context + 0x50; entry < end; entry += 0x0c) {
		const LegoFloat* center = (const LegoFloat*) (entry - 8);
		const LegoFloat* scale = (const LegoFloat*) (entry - 0x50);
		LegoFloat distance = (p_point[0] - center[0]) * scale[0];
		distance += (p_point[1] - center[1]) * scale[1];
		distance += (p_point[2] - center[2]) * scale[2];
		if (!(distance >= p_radius)) {
			return TRUE;
		}
	}
	return FALSE;
}

static void LookupNoOpCleanup()
{
}

const LegoFloat* __fastcall GetLookupResourceOrigin(LegoU8* p_resource, void*);

// STUB: LEGORACERS2 0x004a7510
void __fastcall ApplyLookupResourceState(void* p_owner, void*, void* p_resource)
{
	LegoU8* owner = (LegoU8*) p_owner;
	LegoU8* resource = (LegoU8*) p_resource;
	LegoBool32 selectedByPoint = IsLookupPointWithinRadius(
		(LegoU8*) g_unk0x00679ef0 + 0x270,
		NULL,
		(const LegoFloat*) (resource + 0x9c),
		*(LegoFloat*) (owner + 8)
	);
	LegoS32 profileIndex = !(*(LegoFloat*) (resource + 0xbc) >= 0.2f);
	if (g_unk0x0058a170.m_mode == 1 && !(*(LegoFloat*) (owner + 8) > 25.0f)) {
		profileIndex = 0;
	}
	LegoS32 profileCount = *(LegoS32*) (owner + 0x20);
	if (profileIndex >= profileCount) {
		profileIndex = profileCount - 1;
	}
	LookupProfileRecordView* profile = (LookupProfileRecordView*) (*(LegoU8**) (owner + 0x0c) + profileIndex * 0x14);
	LookupResourceListView* list = *(LookupResourceListView**) (owner + 0x10);
	if (*(LegoU32*) (resource + 0xc0) == 0 || list == NULL) {
		list = (LookupResourceListView*) profile;
	}

	RendererMainDispatch* rendererDispatch = (RendererMainDispatch*) g_displayRenderer->m_vtable;
	LegoU8* ownerPart = *(LegoU8**) (owner + 4);
	typedef void(__fastcall * OwnerPartAction)(void*, void*);
	((OwnerPartAction) (*(void***) ownerPart)[0x2c / 4])(ownerPart, NULL);
	g_displayRenderer->m_unk0x4cc = p_resource;
	rendererDispatch->m_setWorldQuaternion(
		g_displayRenderer,
		NULL,
		(const Quaternion*) (resource + 0x8c),
		(const Vector3*) (resource + 0x9c)
	);
	if (*(LegoU32*) (resource + 0xd0) == 0) {
		LegoU8* target = *(LegoU8**) (resource + 8);
		rendererDispatch->m_selectLights(
			g_displayRenderer,
			NULL,
			(const Vector3*) (resource + 0x9c),
			*(LegoFloat*) (target + 8),
			*(LegoU32*) (target + 4),
			(LegoU32*) (resource + 0xd4)
		);
	}
	else {
		LegoU8* ownerSettings = *(LegoU8**) (owner + 0x14);
		const LegoFloat* origin = GetLookupResourceOrigin(resource, NULL);
		rendererDispatch->m_selectLights(
			g_displayRenderer,
			NULL,
			(const Vector3*) origin,
			*(LegoFloat*) (ownerSettings + 0x70),
			*(LegoU32*) (owner + 4),
			(LegoU32*) (resource + 0xd4)
		);
	}

	if (list != NULL) {
		for (LegoS32 i = 0; i < list->m_count; ++i) {
			LegoU8* item = list->m_items + 0x3c * i;
			RendererDrawPacketView* packet = (RendererDrawPacketView*) item;
			LegoU16* flags = (LegoU16*) (item + 6);
			*flags = (LegoU16) ((*flags & 0xfeff) | (selectedByPoint ? 0 : 0x100));
			LegoS16 originalAuxiliaryIndex = packet->m_auxiliaryRecord;
			LegoBool32 changedAuxiliaryIndex = *(LegoU32*) (resource + 0xa8) != 0xffffffff;
			if (changedAuxiliaryIndex) {
				packet->m_auxiliaryRecord = (LegoS16) * (LegoU32*) (resource + 0xa8);
			}
			RendererAuxiliaryResourceRecord* auxiliary = NULL;
			LegoU32 oldMode = 0;
			LegoFloat oldFactor = 0.0f;
			if (*(LegoU32*) (resource + 0xc0) != 0) {
				auxiliary = g_displayRenderer->GetAuxiliaryResource(packet->m_auxiliaryRecord);
				oldMode = auxiliary->m_mode;
				oldFactor = auxiliary->m_modeFactor;
				auxiliary->m_mode = 1;
				if (oldMode == 1) {
					auxiliary->m_modeFactor = 1.0f - (1.0f - oldFactor) * (1.0f - *(LegoFloat*) (resource + 0xc0));
				}
			}
			LegoU32 resourceFlags = *(LegoU32*) (resource + 0xb4);
			if ((resourceFlags & 0x10) == 0) {
				rendererDispatch->m_drawRendererPacket(g_displayRenderer, NULL, packet);
			}
			else {
				if ((resourceFlags & 0x20) != 0) {
					rendererDispatch->m_drawRendererPacket(g_displayRenderer, NULL, packet);
				}
				if (*(LegoU32*) (resource + 0xac) != 0xffffffff) {
					RendererDefaultRecord savedDefaults = packet->m_defaults;
					packet->m_defaults.m_unk0x00 = 0;
					packet->m_defaults.m_unk0x04 = 0;
					packet->m_defaults.m_unk0x07 = 1;
					packet->m_defaults.m_slots[0].m_value = *(LegoU32*) (resource + 0xac);
					packet->m_defaults.m_slots[0].m_kind = 0;
					packet->m_defaults.m_slots[0].m_flags = 0;
					packet->m_defaults.m_unk0x00 = 1 << (*(LegoU32*) (resource + 0xac) & 31);
					packet->m_defaults.m_unk0x04 = 1;
					rendererDispatch
						->m_resolveTextureStagePlan(g_displayRenderer, NULL, (RendererStagePlanRequest*) packet);
					packet->m_defaults = savedDefaults;
				}
				rendererDispatch->m_drawRendererPacket(g_displayRenderer, NULL, packet);
			}
			if (auxiliary != NULL) {
				auxiliary->m_mode = oldMode;
				auxiliary->m_modeFactor = oldFactor;
			}
			if (changedAuxiliaryIndex) {
				packet->m_auxiliaryRecord = originalAuxiliaryIndex;
			}
		}
	}
}

// STUB: LEGORACERS2 0x004a9d20
const LegoFloat* __fastcall GetLookupResourceOrigin(LegoU8* p_resource, void*)
{
	if ((g_unk0x0058a170.m_selectionOriginInitialized & 1) == 0) {
		g_unk0x0058a170.m_selectionOrigin[0] = 0.0f;
		g_unk0x0058a170.m_selectionOrigin[1] = 0.0f;
		g_unk0x0058a170.m_selectionOrigin[2] = 0.0f;
		g_unk0x0058a170.m_selectionOriginInitialized |= 1;
		atexit(LookupNoOpCleanup);
	}
	LegoU8* object = *(LegoU8**) (p_resource + 0xd0);
	return object != NULL ? (const LegoFloat*) (object + 0x64) : g_unk0x0058a170.m_selectionOrigin;
}

namespace
{

typedef void(__fastcall* LookupObjectAction)(void*, void*, void*);

void InvokeLookupObjectAction(void* p_object, LegoU32 p_slot, void* p_argument)
{
	void** vtable = *(void***) p_object;
	LookupObjectAction action = (LookupObjectAction) vtable[p_slot];
	action(p_object, NULL, p_argument);
}

} // namespace

// STUB: LEGORACERS2 0x004bda20
void DisplayRenderer::DrawLookupEntry(RendererLookupTableState::Entry* p_entry)
{
	RendererMainDispatch* dispatch = (RendererMainDispatch*) m_vtable;
	void* target = (void*) p_entry->m_unk0x08;
	void* resource = (void*) p_entry->m_unk0x0c;
	LegoU32 key = p_entry->m_key;

	if (key == 0) {
		LegoU8* resourceBytes = (LegoU8*) resource;
		LegoU8* targetBytes = (LegoU8*) target;
		LegoU16* targetFlags = (LegoU16*) (targetBytes + 6);
		LegoU16 flags = *targetFlags;
		flags = (LegoU16) ((flags & 0xfeff) | (((~(resourceBytes[0xb4] >> 3)) & 1) << 8));
		*targetFlags = flags;

		if (m_unk0x4c8 != 0) {
			m_unk0x4c8 = 0;
			dispatch->m_applyTextureOperation(this, NULL, 14);
		}

		if (m_unk0x4cc != resource) {
			Quaternion* orientation = (Quaternion*) (resourceBytes + 0x8c);
			Vector3* direction = (Vector3*) (resourceBytes + 0x9c);
			LegoU8* resourceObject = *(LegoU8**) (resourceBytes + 8);
			m_unk0x4cc = resource;
			dispatch->m_setWorldQuaternion(this, NULL, orientation, direction);
			dispatch->m_selectLights(
				this,
				NULL,
				direction,
				*(LegoFloat*) (resourceObject + 8),
				*(LegoU32*) (resourceObject + 4),
				(LegoU32*) (resourceBytes + 0xd4)
			);
		}

		if ((resourceBytes[0xb4] & 0x10) != 0) {
			LegoS16 oldAuxiliary = *(LegoS16*) (targetBytes + 4);
			LegoBool32 changedAuxiliary = *(LegoU32*) (resourceBytes + 0xa8) != 0xffffffff;
			LegoU32 oldMode = 0;
			LegoFloat oldFactor = 0.0f;
			RendererAuxiliaryResourceRecord* auxiliary = NULL;
			if (*(LegoU32*) (resourceBytes + 0xc0) != 0) {
				auxiliary = GetAuxiliaryResource(*(LegoS16*) (targetBytes + 4));
				oldMode = auxiliary->m_mode;
				oldFactor = auxiliary->m_modeFactor;
				auxiliary->m_mode = 1;
				if (oldMode == 1) {
					auxiliary->m_modeFactor = 1.0f - (1.0f - oldFactor) * (1.0f - *(LegoFloat*) (resourceBytes + 0xc0));
				}
			}
			if ((resourceBytes[0xb4] & 0x20) != 0) {
				dispatch->m_drawRendererPacket(this, NULL, (RendererDrawPacketView*) target);
			}
			if (changedAuxiliary) {
				*(LegoU16*) (targetBytes + 4) = (LegoU16) * (LegoU32*) (resourceBytes + 0xa8);
			}
			LegoBool32 changedDefaults = *(LegoU32*) (resourceBytes + 0xac) != 0xffffffff;
			if (changedDefaults) {
				RendererDefaultRecord savedDefaults = ((RendererDrawPacketView*) target)->m_defaults;
				RendererDefaultRecord* defaults = &((RendererDrawPacketView*) target)->m_defaults;
				defaults->m_unk0x00 = 0;
				defaults->m_unk0x04 = 0;
				defaults->m_unk0x07 = 1;
				defaults->m_slots[0].m_value = *(LegoU32*) (resourceBytes + 0xac);
				defaults->m_slots[0].m_kind = 0;
				defaults->m_slots[0].m_flags = 0;
				defaults->m_unk0x00 = 1 << (*(LegoU32*) (resourceBytes + 0xac) & 31);
				defaults->m_unk0x04 = 1;
				dispatch->m_resolveTextureStagePlan(this, NULL, (RendererStagePlanRequest*) target);
				dispatch->m_drawRendererPacket(this, NULL, (RendererDrawPacketView*) target);
				defaults[0] = savedDefaults;
			}
			else {
				dispatch->m_drawRendererPacket(this, NULL, (RendererDrawPacketView*) target);
			}
			if (changedAuxiliary) {
				*(LegoS16*) (targetBytes + 4) = oldAuxiliary;
			}
			if (auxiliary != NULL) {
				auxiliary->m_mode = oldMode;
				auxiliary->m_modeFactor = oldFactor;
			}
			return;
		}

		LegoFloat factor = *(LegoFloat*) (resourceBytes + 0xc0);
		if (factor == 0.0f) {
			dispatch->m_drawRendererPacket(this, NULL, (RendererDrawPacketView*) target);
			return;
		}

		RendererAuxiliaryResourceRecord* auxiliary = GetAuxiliaryResource(*(LegoS16*) (targetBytes + 4));
		LegoU32 oldMode = auxiliary->m_mode;
		LegoFloat oldFactor = auxiliary->m_modeFactor;
		auxiliary->m_mode = 1;
		if (oldMode == 1) {
			auxiliary->m_modeFactor = 1.0f - (1.0f - oldFactor) * (1.0f - factor);
		}
		dispatch->m_drawRendererPacket(this, NULL, (RendererDrawPacketView*) target);
		auxiliary->m_mode = oldMode;
		auxiliary->m_modeFactor = oldFactor;
		return;
	}

	if (key == 1) {
		LegoU8* resourceBytes = (LegoU8*) resource;
		LegoU8* object = *(LegoU8**) (resourceBytes + 8);
		*(LegoFloat*) (resourceBytes + 0xbc) = dispatch->m_scaleProjectionCoordinate(
			this,
			NULL,
			*(LegoFloat*) (object + 8),
			*(LegoFloat*) (resourceBytes + 0xb8)
		);
		if (m_unk0x4c8 != 0) {
			m_unk0x4c8 = 0;
			dispatch->m_applyTextureOperation(this, NULL, 14);
		}
		ApplyLookupResourceState(object, NULL, resource);
		m_unk0x4cc = resource;
		return;
	}

	if (key == 2 || key == 3) {
		LegoU32 state = key == 2 ? 5 : 7;
		if (m_unk0x4c8 != state) {
			m_unk0x4c8 = state;
			dispatch->m_applyTextureOperation(this, NULL, 14);
			dispatch->m_applyTextureOperation(this, NULL, state);
		}
		m_unk0x4cc = NULL;
		if (key == 2) {
			dispatch->m_drawSprite(this, NULL, (RendererSpriteRecord*) target);
		}
		else {
			dispatch->m_drawQuadPacket(this, NULL, target);
		}
		return;
	}

	if (key == 4 || key == 5) {
		if (m_unk0x4c8 != 0) {
			m_unk0x4c8 = 0;
			dispatch->m_applyTextureOperation(this, NULL, 14);
		}
		m_unk0x4cc = NULL;
		InvokeLookupObjectAction(target, key == 4 ? 3 : 13, resource);
		m_unk0x4c8 = 0;
	}
}
