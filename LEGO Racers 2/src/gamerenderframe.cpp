#include "gamerenderframe.h"

#include "gameobjectgeometry.h"

#include <stddef.h>

namespace
{

struct ChildGeometryOwnerView {
	undefined m_opaque0x00[0x48];
	void* m_childGeometry;
};

DECOMP_STATIC_ASSERT(offsetof(ChildGeometryOwnerView, m_childGeometry) == 0x48);

} // namespace

// STUB: LEGORACERS2 0x004e9390
void __fastcall ForwardPositionToChildGeometry(void* p_owner, void*, const LegoFloat* p_position)
{
	ChildGeometryOwnerView* owner = (ChildGeometryOwnerView*) p_owner;
	((GameObjectChildGeometry*) owner->m_childGeometry)->SetPosition(p_position);
}
