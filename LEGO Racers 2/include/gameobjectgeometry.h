#ifndef LR2_GAMEOBJECTGEOMETRY_H
#define LR2_GAMEOBJECTGEOMETRY_H

#include "types.h"

// The recovered child transform object has additional fields used by its
// implementation. This interface exposes only the two observed callbacks.
class GameObjectChildGeometry {
public:
	void SetPosition(const LegoFloat* p_position);
	void SetRotation(const LegoFloat* p_rotation);
};

#endif // LR2_GAMEOBJECTGEOMETRY_H
