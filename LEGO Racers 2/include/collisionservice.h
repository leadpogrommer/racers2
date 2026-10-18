#ifndef LR2_COLLISIONSERVICE_H
#define LR2_COLLISIONSERVICE_H

#include "collisionproxy.h"
#include "collisionspatialindex.h"

void InitializeCollisionServices();
void __cdecl DetachCollisionProxy(CollisionProxy* p_proxy);
void __cdecl AttachCollisionProxy(CollisionProxy* p_proxy);

struct CollisionServiceObject;
struct CollisionQueryResult;
struct CollisionServiceDispatch {
	typedef CollisionServiceObject*(__fastcall* Destroy)(CollisionServiceObject*, void*, LegoU32);
	typedef LegoU32(__fastcall* PrepareQuery)(void*, void*, LegoU32*);
	typedef LegoU32(__fastcall* QuerySphere)(void*, void*, const Vector3*, LegoFloat, LegoU32*);
	typedef LegoU32(__fastcall* QuerySegment)(void*, void*, const Vector3*, const Vector3*, CollisionQueryResult*);
	typedef void(__fastcall* SetTransform)(void*, void*, CollisionProxy*, const Vector3*, const Matrix3*, LegoS32);
	typedef void(__fastcall* SetPrimitivePosition)(void*, void*, CollisionProxy*, LegoS32, const Vector3*, LegoU32);
	typedef void(__fastcall* DetachAll)(void*, void*);
	typedef LegoFloat(__fastcall* Sweep)(void*, void*, CollisionProxy*, const Vector3*, LegoFloat);

	Destroy m_destroy;
	PrepareQuery m_prepareQuery;
	QuerySphere m_querySphere;
	QuerySegment m_querySegment;
	SetTransform m_setTransform;
	SetPrimitivePosition m_setPrimitivePosition;
	DetachAll m_detachAll;
	Sweep m_sweep;
};

struct CollisionServiceObject {
	const CollisionServiceDispatch* m_vtable;
};

extern CollisionServiceObject* g_collisionProxyService;
extern CollisionSpatialIndex* g_collisionSpatialIndex;

#endif // LR2_COLLISIONSERVICE_H
