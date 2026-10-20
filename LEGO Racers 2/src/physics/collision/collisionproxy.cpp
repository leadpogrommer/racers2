#include "physics/collision/collisionproxy.h"

#include "game/objects/gamemanager.h"
#include "memory/memoryheap.h"

#include <string.h>

// STUB: LEGORACERS2 0x004d7eb0
CollisionProxy::CollisionProxy(CollisionModelView* p_model, LegoU32 p_tag)
{
	m_model = p_model;
	((CollisionModelTransform*) &m_position)->ResetIdentity();
	m_unk0x38 = 0;
	m_indexLink.Initialize(this);
	memset(m_unk0x70, 0, sizeof(m_unk0x70));
	m_tag = p_tag;
	if (m_model != NULL) {
		m_children = m_model->m_childDefinitions.CreateProxies(this, &m_childCount);
		m_parent = NULL;
		m_transformIndex = 0;
		m_complexity = 0;
		if (m_model != NULL) {
			if (m_model->m_unk0x08 != 0) {
				m_complexity = 100000000;
			}
			if (m_model->m_unk0x04 != 0) {
				m_complexity += 100000;
			}
			m_complexity += m_model->m_primitiveCount;
			m_complexity += m_model->m_unk0x18 * 8;
		}
	}
}

// STUB: LEGORACERS2 0x004d7fa0
CollisionProxy::~CollisionProxy()
{
	if (m_children != NULL) {
		for (LegoS32 i = m_childCount - 1; i >= 0; i--) {
			DestroyCollisionProxy(&m_children[i]);
		}
		FreeHeapMemory(m_children);
		m_childCount = 0;
	}
	m_indexLink.Destroy();
}

// STUB: LEGORACERS2 0x004d7ff0
void CollisionProxy::SetTransform(const Vector3* p_position, const Matrix3* p_rotation, const LegoS32* p_filter)
{
	m_position = *p_position;
	m_rotation = *p_rotation;
	m_indexLink.SetFilter(p_filter);
	m_indexLink.Update();
	for (LegoS32 i = 0; i < m_childCount; i++) {
		CollisionProxy* child = m_children[i];
		const CollisionModelTransform* transform =
			&m_model->m_childDefinitions.m_transforms[child->m_transformIndex & 0x7fffffff];
		child->SetChildTransform((const CollisionModelTransform*) &m_position, transform, p_filter);
	}
}

// STUB: LEGORACERS2 0x004d8090
void CollisionProxy::SetChildTransform(
	const CollisionModelTransform* p_parent,
	const CollisionModelTransform* p_child,
	const LegoS32* p_filter
)
{
	m_position = p_child->m_position;
	m_position.FUN_004a4e00(&p_parent->m_rotation);
	m_rotation.FUN_004a4ea0(&p_parent->m_rotation, &p_child->m_rotation);
	m_position.m_x += p_parent->m_position.m_x;
	m_position.m_y += p_parent->m_position.m_y;
	m_position.m_z += p_parent->m_position.m_z;
	m_indexLink.SetFilter(p_filter);
	m_indexLink.Update();
}

// STUB: LEGORACERS2 0x004d8110
void CollisionProxy::DetachFromIndex()
{
	m_indexLink.Detach();
	for (LegoS32 i = 0; i < m_childCount; i++) {
		m_children[i]->DetachFromIndex();
	}
}

// STUB: LEGORACERS2 0x004d8150
void CollisionProxy::AttachToIndex()
{
	m_indexLink.Attach();
	for (LegoS32 i = 0; i < m_childCount; i++) {
		m_children[i]->AttachToIndex();
	}
}

// STUB: LEGORACERS2 0x004d8300
void CollisionProxy::SetPrimitivePosition(LegoS32 p_index, const Vector3* p_position, LegoU32 p_unused)
{
	// Preserve the retail inclusive upper bound (JG, rather than JGE).
	if (p_index >= 0 && p_index <= m_model->m_primitiveCount) {
		m_model->m_primitives[p_index].m_position = *p_position;
	}
}

// STUB: LEGORACERS2 0x004e6a30
void GameObject::RefreshCollisionState()
{
	GameLifecycleComponent* component = m_lifecycle0x54;
	LegoS32 filter;
	if ((component->m_stateFlags & 1) != 0) {
		filter = -2;
	}
	else {
		LegoBool32 active = component->m_vtable->m_isActive(component, NULL, 0);
		if (active && ((component->m_owner->m_flags & 2) != 0 || ((component->m_owner->m_flags >> 3) & 2) != 0)) {
			filter = -1;
		}
		else {
			typedef LegoS32(__fastcall * GetFilter)(GameLifecycleComponent*, void*);
			GetFilter getFilter = *(GetFilter*) ((LegoU8*) component->m_vtable + 4);
			filter = getFilter(component, NULL);
		}
	}

	typedef const Matrix3*(__fastcall * GetRotation)(GameLifecycleComponent*, void*);
	typedef const Vector3*(__fastcall * GetPosition)(GameLifecycleComponent*, void*);
	GetRotation getRotation = *(GetRotation*) ((LegoU8*) component->m_vtable + 0x18);
	GetPosition getPosition = *(GetPosition*) ((LegoU8*) component->m_vtable + 0x14);
	const Matrix3* rotation = getRotation(component, NULL);
	const Vector3* position = getPosition(component, NULL);
	component->m_collisionProxy->SetTransform(position, rotation, &filter);
}

// STUB: LEGORACERS2 0x004f50e0
CollisionModelTransform* CollisionModelTransform::ResetIdentity()
{
	m_position.m_x = 0.0f;
	m_position.m_y = 0.0f;
	m_position.m_z = 0.0f;
	for (LegoU32 row = 0; row < 3; row++) {
		for (LegoU32 column = 0; column < 3; column++) {
			m_rotation.m_values[row][column] = row == column ? 1.0f : 0.0f;
		}
	}
	return this;
}

// STUB: LEGORACERS2 0x004fa7f0
CollisionProxy** CollisionChildDefinitions::CreateProxies(CollisionProxy* p_parent, LegoS32* p_count)
{
	if (m_count == 0) {
		*p_count = 0;
		return NULL;
	}
	*p_count = m_count;
	CollisionProxy** children = (CollisionProxy**) AllocateHeapMemory((LegoU32) m_count * sizeof(CollisionProxy*));
	for (LegoS32 i = 0; i < m_count; i++) {
		CollisionProxy* child = CreateCollisionProxy(m_models[m_modelIndices[i]], p_parent->m_tag);
		children[i] = child;
		child->m_parent = p_parent;
		child->m_transformIndex = (child->m_transformIndex & 0x80000000) | (LegoU32) i;
		LegoS32 filter = p_parent->m_indexLink.m_filter;
		child->SetChildTransform((const CollisionModelTransform*) &p_parent->m_position, &m_transforms[i], &filter);
	}
	return children;
}

// STUB: LEGORACERS2 0x004fb4f0
SpatialIndexLink* SpatialIndexLink::Initialize(CollisionProxy* p_proxy)
{
	memset(this, 0, sizeof(*this));
	m_index = -1;
	m_proxy = p_proxy;
	return this;
}

// STUB: LEGORACERS2 0x004fb680
void SpatialIndexLink::Destroy()
{
	Detach();
}
