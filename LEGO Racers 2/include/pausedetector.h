#ifndef LR2_PAUSEDETECTOR_H
#define LR2_PAUSEDETECTOR_H

#include "gamemanager.h"

class PauseDetector : public GameObject {
public:
	PauseDetector(GameObjectTemplate* p_template);
};

DECOMP_SIZE_ASSERT(PauseDetector, 0x58);

LegoU32 __fastcall PauseDetectorUpdate(GameObject* p_object, void* p_unusedEdx, LegoU32 p_frameParameter);

#endif // LR2_PAUSEDETECTOR_H
