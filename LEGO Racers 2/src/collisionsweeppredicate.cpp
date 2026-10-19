#include "collisionqueries.h"

// FUNCTION: LEGORACERS2 0x004047a0 FOLDED
LegoBool32 __stdcall IsCollisionSweepPositionClear(CollisionProxy*)
{
	// The supplied executable contains this literal return-zero/RET 4 leaf.
	// Its unused sweep caller therefore cannot make progress for small steps.
	return FALSE;
}
