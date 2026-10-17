#include "renderer.h"

namespace
{
// Common identity prefix of the real texture and animation records. The
// records share this observed ABI; this view adds no object or virtual table.
struct ResourceIdentity {
	const void* m_vtable;
	LegoS16 m_id;
	LegoU16 m_flags;
};
DECOMP_SIZE_ASSERT(ResourceIdentity, 8);

struct ResourceLifetimeDispatch {
	undefined m_unk0x00[0x0c];
	void(__fastcall* m_release)(void*, void*);
	LegoBool32(__fastcall* m_isAnimated)(void*, void*);
};
} // namespace

// STUB: LEGORACERS2 0x004c0ae0
RendererResourceRef* DisplayRenderer::FindTextureReferenceById(LegoS32 p_id, LegoU32 p_type)
{
	for (LegoS32 i = 0; i < m_resourceReferenceCount; i++) {
		RendererResourceRef* reference = &m_resourceReferences[i];
		if (((ResourceIdentity*) reference->m_resource)->m_id == p_id && reference->m_resourceType == p_type) {
			return reference;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004c0ee0
void DisplayRenderer::ReleaseTextureResourceReference(LegoS32 p_id, LegoU32 p_type)
{
	if (p_id == -1) {
		return;
	}
	RendererResourceRef* reference = FindTextureReferenceById(p_id, p_type);
	if (reference == NULL) {
		return;
	}
	reference->m_referenceCount--;
	if (reference->m_referenceCount != 0) {
		return;
	}
	ResourceIdentity* resource = (ResourceIdentity*) reference->m_resource;
	if ((resource->m_flags & 1) != 0) {
		const ResourceLifetimeDispatch* dispatch = (const ResourceLifetimeDispatch*) resource->m_vtable;
		if (dispatch->m_isAnimated(resource, NULL) != 0) {
			for (LegoS32 animated = 0; animated < m_resourceCount0x9c; animated++) {
				if (m_resources0x98[animated] == reference) {
					m_resourceCount0x9c--;
					m_resources0x98[animated] = m_resources0x98[m_resourceCount0x9c];
					break;
				}
			}
		}
		if (m_unk0x4dc == 1) {
			for (LegoS32 cached = 0; cached < 6; cached++) {
				if (m_cachedTextureRecordIds[cached] == resource->m_id) {
					m_unk0x4dc = 0;
					break;
				}
			}
		}
		const LegoU32 hash = (resource->m_flags >> 6) & 0xff;
		if ((void*) m_textureHash[hash] == resource) {
			m_textureHash[hash] = NULL;
		}
		dispatch = (const ResourceLifetimeDispatch*) resource->m_vtable;
		dispatch->m_release(resource, NULL);
		// An animation's callback can release other references recursively,
		// moving entries. Retail looks this reference up again afterwards.
		reference = FindTextureReferenceById(p_id, p_type);
		((ResourceIdentity*) reference->m_resource)->m_flags &= 0xfffe;
	}
	RendererResourceRef* last = &m_resourceReferences[m_resourceReferenceCount - 1];
	if (reference < last) {
		*reference = *last;
		ResourceIdentity* moved = (ResourceIdentity*) last->m_resource;
		const ResourceLifetimeDispatch* dispatch = (const ResourceLifetimeDispatch*) moved->m_vtable;
		if (dispatch->m_isAnimated(moved, NULL) != 0) {
			for (LegoS32 index = 0; index < m_resourceCount0x9c; index++) {
				if (m_resources0x98[index] == last) {
					m_resources0x98[index] = reference;
					break;
				}
			}
		}
	}
	m_resourceReferenceCount--;
}
