#include "collisionservice.h"

#include "collisionqueries.h"
#include "memoryheap.h"

#include <new.h>

extern "C" int __cdecl _purecall(void);

CollisionServiceObject* __fastcall DestroyCollisionServiceObject(
	CollisionServiceObject* p_service,
	void*,
	LegoU32 p_flags
);
LegoU32 __fastcall CollisionServicePrepareQuery(void*, void*, LegoU32* p_result);
LegoU32 __fastcall CollisionServiceQuerySphere(
	void*,
	void*,
	const Vector3* p_center,
	LegoFloat p_radius,
	LegoU32* p_tags
);
LegoU32 __fastcall CollisionServiceQuerySegment(
	void*,
	void*,
	const Vector3* p_start,
	const Vector3* p_end,
	CollisionQueryResult* p_result
);
void __fastcall CollisionServiceSetTransform(
	void*,
	void*,
	CollisionProxy* p_proxy,
	const Vector3* p_position,
	const Matrix3* p_rotation,
	LegoS32 p_filter
);
void __fastcall CollisionServiceSetPrimitivePosition(
	void*,
	void*,
	CollisionProxy* p_proxy,
	LegoS32 p_index,
	const Vector3* p_position,
	LegoU32 p_unused
);
void __fastcall CollisionServiceDetachAll(void*, void*);
LegoFloat __fastcall CollisionServiceSweep(
	void*,
	void*,
	CollisionProxy* p_proxy,
	const Vector3* p_direction,
	LegoFloat p_step
);
CollisionProxy* __fastcall CollisionProxyFromService(
	CollisionServiceObject*,
	void*,
	CollisionModelView* p_model,
	LegoU32 p_tag
);
LegoU32 __fastcall DestroyCollisionProxyFromService(CollisionServiceObject*, void*, CollisionProxy** p_proxy);
void __fastcall AttachCollisionProxyFromService(CollisionServiceObject*, void*, CollisionProxy* p_proxy);

// GLOBAL: LEGORACERS2 0x00535fa0
extern const CollisionServiceDispatch g_collisionProxyServiceDispatch = {
	DestroyCollisionServiceObject,
	CollisionServicePrepareQuery,
	CollisionServiceQuerySphere,
	CollisionServiceQuerySegment,
	CollisionServiceSetTransform,
	CollisionServiceSetPrimitivePosition,
	CollisionServiceDetachAll,
	CollisionServiceSweep,
};

struct CollisionServiceAbstractDispatch {
	typedef void(__fastcall* Purecall)(void*, void*);
	CollisionServiceDispatch::Destroy m_destroy;
	Purecall m_abstract[7];
};

// GLOBAL: LEGORACERS2 0x00535fc0
static const CollisionServiceAbstractDispatch g_collisionProxyServiceAbstractDispatch = {
	DestroyCollisionServiceObject,
	{(CollisionServiceAbstractDispatch::Purecall) _purecall,
	 (CollisionServiceAbstractDispatch::Purecall) _purecall,
	 (CollisionServiceAbstractDispatch::Purecall) _purecall,
	 (CollisionServiceAbstractDispatch::Purecall) _purecall,
	 (CollisionServiceAbstractDispatch::Purecall) _purecall,
	 (CollisionServiceAbstractDispatch::Purecall) _purecall,
	 (CollisionServiceAbstractDispatch::Purecall) _purecall},
};

// STUB: LEGORACERS2 0x004b95e0
void InitializeCollisionServices()
{
	if (g_collisionProxyService == NULL) {
		void* storage = AllocateHeapMemory(sizeof(CollisionServiceObject));
		if (storage == NULL) {
			g_collisionProxyService = NULL;
		}
		else {
			g_collisionProxyService = (CollisionServiceObject*) storage;
			g_collisionProxyService->m_vtable = &g_collisionProxyServiceDispatch;
		}
	}

	if (g_collisionSpatialIndex == NULL) {
		void* storage = AllocateHeapMemory(sizeof(CollisionSpatialIndex));
		if (storage == NULL) {
			g_collisionSpatialIndex = NULL;
		}
		else {
			Vector3 origin;
			origin.m_x = 0.0f;
			origin.m_y = 0.0f;
			origin.m_z = 0.0f;
			g_collisionSpatialIndex = new (storage) CollisionSpatialIndex(0x20, &origin, 725.0f, 15000, 0x100, 0x80);
		}
	}

	if (g_collisionServiceState == NULL) {
		void* storage = AllocateHeapMemory(sizeof(CollisionPairQueryState));
		if (storage == NULL) {
			g_collisionServiceState = NULL;
		}
		else {
			g_collisionServiceState = (CollisionPairQueryState*) storage;
			g_collisionServiceState->m_firstTransform.ResetIdentity();
			g_collisionServiceState->m_secondTransform.ResetIdentity();
			g_collisionServiceState->m_currentLinkCount = 0;
			g_collisionServiceState->m_unk0x4c04 = 0;
			g_collisionServiceState->m_unk0x4c08 = 0;
		}
	}
}

// STUB: LEGORACERS2 0x004b96d0
CollisionServiceObject* __fastcall DestroyCollisionServiceObject(
	CollisionServiceObject* p_service,
	void*,
	LegoU32 p_flags
)
{
	p_service->m_vtable = (const CollisionServiceDispatch*) &g_collisionProxyServiceAbstractDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_service);
	}
	return p_service;
}

// STUB: LEGORACERS2 0x004b9810
CollisionProxy* __cdecl CreateCollisionProxy(CollisionModelView* p_model, LegoU32 p_tag)
{
	InitializeCollisionServices();
	return CollisionProxyFromService(g_collisionProxyService, NULL, p_model, p_tag);
}

// STUB: LEGORACERS2 0x004b9850
void __cdecl AttachCollisionProxy(CollisionProxy* p_proxy)
{
	InitializeCollisionServices();
	AttachCollisionProxyFromService(g_collisionProxyService, NULL, p_proxy);
}

// STUB: LEGORACERS2 0x004b9870
LegoU32 __cdecl DestroyCollisionProxy(CollisionProxy** p_proxy)
{
	InitializeCollisionServices();
	return DestroyCollisionProxyFromService(g_collisionProxyService, NULL, p_proxy);
}

// STUB: LEGORACERS2 0x004f4c50
void __fastcall CollisionServiceDetachAll(void*, void*)
{
	struct RegistryEntryView {
		SpatialIndexLink* m_link;
		undefined m_unk0x04[4];
		RegistryEntryView* m_next;
	};

	RegistryEntryView* entry = *(RegistryEntryView**) ((LegoU8*) g_collisionSpatialIndex->m_registry + 4);
	while (entry != NULL) {
		RegistryEntryView* next = entry->m_next;
		entry->m_link->m_proxy->DetachFromIndex();
		entry = next;
	}
}

// STUB: LEGORACERS2 0x004f4c80
void __fastcall CollisionServiceSetPrimitivePosition(
	void*,
	void*,
	CollisionProxy* p_proxy,
	LegoS32 p_index,
	const Vector3* p_position,
	LegoU32 p_unused
)
{
	if (p_proxy != NULL) {
		p_proxy->SetPrimitivePosition(p_index, p_position, p_unused);
	}
}

// STUB: LEGORACERS2 0x004f4ca0
void __fastcall CollisionServiceSetTransform(
	void*,
	void*,
	CollisionProxy* p_proxy,
	const Vector3* p_position,
	const Matrix3* p_rotation,
	LegoS32 p_filter
)
{
	if (p_proxy != NULL) {
		p_proxy->SetTransform(p_position, p_rotation, &p_filter);
	}
}

// STUB: LEGORACERS2 0x004f4cf0
CollisionProxy* __fastcall CollisionProxyFromService(
	CollisionServiceObject*,
	void*,
	CollisionModelView* p_model,
	LegoU32 p_tag
)
{
	if (p_model == NULL) {
		return NULL;
	}

	void* storage = AllocateHeapMemory(sizeof(CollisionProxy));
	if (storage == NULL) {
		return NULL;
	}
	return new (storage) CollisionProxy(p_model, p_tag);
}

// STUB: LEGORACERS2 0x004f4dc0
LegoU32 __fastcall DestroyCollisionProxyFromService(CollisionServiceObject*, void*, CollisionProxy** p_proxy)
{
	CollisionProxy* proxy = *p_proxy;
	if (proxy != NULL) {
		proxy->~CollisionProxy();
		FreeHeapMemory(proxy);
	}
	*p_proxy = NULL;
	return 0;
}

// STUB: LEGORACERS2 0x004f4e00
void __fastcall AttachCollisionProxyFromService(CollisionServiceObject*, void*, CollisionProxy* p_proxy)
{
	if (p_proxy != NULL) {
		p_proxy->AttachToIndex();
	}
}

// STUB: LEGORACERS2 0x004f4e40
LegoU32 __fastcall CollisionServicePrepareQuery(void*, void*, LegoU32* p_result)
{
	PrepareCollisionSpatialQueries(g_collisionSpatialIndex, NULL);
	*p_result = (LegoU32) g_collisionServiceState;
	return g_collisionServiceState->m_currentLinkCount;
}

// STUB: LEGORACERS2 0x004f4e70
LegoU32 __fastcall CollisionServiceQuerySphere(
	void*,
	void*,
	const Vector3* p_center,
	LegoFloat p_radius,
	LegoU32* p_tags
)
{
	return QueryCollisionSphere(g_collisionSpatialIndex, NULL, p_center, p_radius, p_tags);
}

// STUB: LEGORACERS2 0x004f4e90
LegoU32 __fastcall CollisionServiceQuerySegment(
	void*,
	void*,
	const Vector3* p_start,
	const Vector3* p_end,
	CollisionQueryResult* p_result
)
{
	return QueryCollisionSegment(g_collisionSpatialIndex, NULL, p_start, p_end, p_result);
}

// STUB: LEGORACERS2 0x004f4eb0
LegoFloat __fastcall CollisionServiceSweep(
	void*,
	void*,
	CollisionProxy* p_proxy,
	const Vector3* p_direction,
	LegoFloat p_step
)
{
	if (p_proxy == NULL) {
		return 0.0f;
	}
	if (!(p_step >= 0.0001f)) {
		p_step = 0.0001f;
	}
	Vector3* position = &p_proxy->m_position;
	const LegoFloat oldX = position->m_x;
	const LegoFloat oldY = position->m_y;
	const LegoFloat oldZ = position->m_z;
	LegoFloat distance = 0.0f;
	LegoFloat step = 0.1f;
	while (p_step <= step) {
		if (IsCollisionSweepPositionClear(p_proxy) == FALSE) {
			do {
				distance += step;
				position->m_x += -step * p_direction->m_x;
				position->m_y += -step * p_direction->m_y;
				position->m_z += -step * p_direction->m_z;
			} while (IsCollisionSweepPositionClear(p_proxy) == FALSE);
		}
		distance -= step;
		position->m_x += -step * p_direction->m_x;
		position->m_y += -step * p_direction->m_y;
		position->m_z += -step * p_direction->m_z;
		step *= 0.5f;
	}

	position->m_x = oldX;
	position->m_y = oldY;
	position->m_z = oldZ;
	return distance;
}

// STUB: LEGORACERS2 0x004f5370
void __fastcall PrepareCollisionSpatialQueries(CollisionSpatialIndex* p_index, void*)
{
	g_collisionServiceState->m_currentLinkCount = 0;
	p_index->m_staticGrid->PrepareQuery(p_index->m_dynamicIndex);
	p_index->m_dynamicIndex->ResetQuery();
}
